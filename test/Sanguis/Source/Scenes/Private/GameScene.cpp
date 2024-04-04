//
// Created by EMCJava on 4/3/2024.
//

#include "GameScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/Audio/AudioEngine.hpp>

#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Camera/FirstPersonCameraController.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshCluster.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>

#include <Engine/Core/UI/Text/Text.hpp>

#include <Engine/Core/Scene/Entity/EntityRenderable.hpp>

#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Input/UserInput.hpp>

#include <Engine/Core/Physics/Controller/PhyControllerEntityPlayer.hpp>
#include <Engine/Core/Physics/Controller/PhyControllerEntity.hpp>
#include <Engine/Core/Physics/RigidBody/RigidMesh.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>

#include <PxPhysicsAPI.h>

#include <ranges>
#include <utility>

DEFINE_CONCEPT_DS( GameScene )

class Reticle : public ConceptList
{
    DECLARE_CONCEPT( Reticle, ConceptList )

public:
    Reticle( )
    {
        SetSearchThrough( true );

        auto FixedCamera = AddConcept<PureConceptOrthoCamera>( ).Get( );

        FixedCamera->SetScale( 1 / 1.5F );
        FixedCamera->UpdateCameraMatrix( );

        auto UICanvas = AddConcept<Canvas>( ).Get( );
        UICanvas->SetRuntimeName( "UI Canvas" );
        UICanvas->SetCanvasCamera( FixedCamera );

        auto TextureShader = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" );
        m_ReticleSprite    = UICanvas->AddConcept<SpriteSquareTexture>( TextureShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/reticle-red-dot.png" ) ).Get( );
        m_ReticleSprite->SetCenterAsOrigin( );
        m_ReticleSprite->SetScale( glm::vec3 { 0.4 } );
    }

    auto&
    GetSprite( ) { return m_ReticleSprite; }

protected:
    std::shared_ptr<SpriteSquareTexture> m_ReticleSprite;
};
DEFINE_CONCEPT_DS( Reticle )

class PostEntityUpdateWrapper : public ConceptList
{
    DECLARE_CONCEPT( PostEntityUpdateWrapper, ConceptList )
public:
    struct EntityNetOrientation {
        glm::dvec3 FootPosition;
        glm::vec3  Velocity;
    };

    PostEntityUpdateWrapper( std::shared_ptr<PhyControllerEntity> Entity, std::shared_ptr<SanguisNet::ClientGroupParticipant>& Connection, bool ReportPosition )
        : m_Entity( std::move( Entity ) )
        , m_Connection( Connection )
        , m_DoReport( ReportPosition )
    {
        SetSearchThrough( true );
        if ( !m_DoReport )
        {
            GetOwnership( m_Entity );

            auto RMC = SerializerModel::ToMeshCluster( "Assets/Model/PlayerCapsule.glb", eFeatureDefault | eFeatureUV0 );
            RMC->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
            RMC->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
            RMC->SetShaderUniform( "viewPos", glm::vec3( 1 ) );
            RMC->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );

            m_Entity->SetSearchThrough( true );
            auto ER = m_Entity->AddConcept<EntityRenderable>( RMC ).Get( );
            ER->SetRuntimeName( "Map" );

            ER->UpdateOrientation( { } );
        }
    }

    void Apply( ) override
    {
        if ( m_DoReport )
        {
            if ( !m_Entity->HasMoved( ) ) return;
            m_Connection->Post( SanguisNet::Message::FromStruct(
                EntityNetOrientation {
                    .FootPosition = m_Entity->GetFootPosition( ),
                    .Velocity     = m_Entity->GetVelocity( ) },
                SanguisNet::MessageHeader::ID_GAME_UPDATE_SELF_COORDINATES ) );
        }
    }

    void SetNetOrientation( const EntityNetOrientation& NO )
    {
        m_Entity->SetFootPosition( NO.FootPosition );
        m_Entity->SetVelocity( NO.Velocity );
    }

protected:
    std::shared_ptr<SanguisNet::ClientGroupParticipant>& m_Connection;
    std::shared_ptr<PhyControllerEntity>                 m_Entity;
    bool                                                 m_DoReport = false;
};
DEFINE_CONCEPT_DS( PostEntityUpdateWrapper )

GameScene::GameScene( std::shared_ptr<SanguisNet::ClientGroupParticipant> Connection, std::string Name )
    : m_ServerConnection( std::move( Connection ) )
    , m_UserName( std::move( Name ) )
{
    SetSearchThrough( );

    {
        m_MainCamera = AddConcept<PureConceptPerspectiveCamera>( );

        m_MainCamera->SetRuntimeName( "Main Camera" );
        m_MainCamera->PushToCameraStack( );

        m_MainCamera->SetCameraPerspectiveFOV( 45, false );
        m_MainCamera->SetCameraPosition( glm::vec3( 0.0f, 3.0f, 3.0f ), false );
        m_MainCamera->SetCameraFacing( glm::vec3( 0.0f, -0.70710678118F, -0.70710678118F ), false );

        m_MainCamera->SetCameraPosition( glm::vec3( -22.228, 14.06, 10.178 ), false );
        m_MainCamera->SetCameraPrincipalAxes( -25.25, -29 );
    }

    {
        const auto& PerspectiveCanvas = AddConcept<Canvas>( ).Get( );
        PerspectiveCanvas->SetRuntimeName( "Perspective Canvas" );
        PerspectiveCanvas->SetCanvasCamera( m_MainCamera );

        {
            {
                auto* PhyEngine = Engine::GetEngine( )->GetPhysicsEngine( );

                m_DefaultPhyMaterial              = ( *PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *PhyEngine, physx::PxPlane( 0, 1, 0, 0 ), *m_DefaultPhyMaterial );
                Engine::GetEngine( )->AddPhysicsCallback( [ Actor = groundPlane ]( auto* PhyEngine ) {
                    PhyEngine->GetScene( )->addActor( *Actor );
                } );

                const auto RenderableShaderSetup = [ this ]( auto& Renderable, const std::string& ShaderName ) {
                    Renderable->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( ShaderName ) );
                    Renderable->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
                    Renderable->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
                    Renderable->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
                };

                {
                    auto                       CM = SerializerModel::ToMesh( "Assets/Model/low-poly_fps_map/scene.gltf", glm::scale( glm::mat4 { 1 }, glm::vec3 { 0.35 } ) );
                    std::shared_ptr<RigidMesh> RM = PerspectiveCanvas->AddConcept<RigidMesh>( CM, CreateConcept<ColliderMesh>( CM, m_DefaultPhyMaterial, true, []( auto& SubMeshSpan ) {
                                                                                                  return ColliderSerializerGroupMesh::ColliderType::eTriangle;
                                                                                              } ) );

                    RM->SetRuntimeName( "Map" );
                    RM->RemoveConcept<EntityRenderable>( );

                    auto RMC = SerializerModel::ToMeshCluster( "Assets/Model/low-poly_fps_map/scene.gltf", eFeatureDefault | eFeatureUV0 );
                    RenderableShaderSetup( RMC, "DefaultPhongShader" );

                    auto ER = AddConcept<EntityRenderable>( RMC ).Get( );
                    ER->SetRuntimeName( "Map" );

                    Orientation Ori;
                    Ori.Scale = glm::vec3 { 0.35 };
                    ER->UpdateOrientation( Ori );
                    RM->GetOwnership( ER );
                    RM->GetConcept<EntityRenderable>( )->MoveToLastAsSubConcept( );
                }

                // Controller
                {
                    m_CharController = AddConcept<PhyControllerEntityPlayer>( m_MainCamera );
                    auto Lock        = Engine::GetEngine( )->GetPhysicsThreadLock( );
                    m_CharController->CreateController( { 0, 100, 0 }, 1.f, 0.3f, m_DefaultPhyMaterial );
                    m_CharController->SetSceneQuery( false );
                }
            }
        }
    }

    {
        auto FixedCamera = AddConcept<PureConceptOrthoCamera>( ).Get( );

        FixedCamera->SetScale( 1 / 1.5F );
        FixedCamera->UpdateCameraMatrix( );

        auto UICanvas = AddConcept<Canvas>( ).Get( );
        UICanvas->SetRuntimeName( "UI Canvas" );
        UICanvas->SetCanvasCamera( FixedCamera );

        m_HealthText = UICanvas->AddConcept<Text>( "100" ).Get( );
        m_HealthText->SetupSprite( );
        m_HealthText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
        m_HealthText->SetColor( glm::vec3 { 1 } );
        m_HealthText->SetCoordinate( glm::vec3 { -1400, 760, 0 } );
    }

    m_GunFireAudioHandle = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/GunFire.ogg" );

    // 373
    m_Reticle = AddConcept<Reticle>( );

    m_CameraZoomLerp.Start = /* Zoom FOV */ 30;
    m_CameraZoomLerp.End   = /* Default FOV */ 45;
    m_CameraZoomLerp.SetOneRoundTime( 0.06 );
    m_CameraZoomLerp.Update( 0.06 - 0.0001 );   // Activate once

    m_ServerConnection->SetPacketCallback( [ this ]( auto&& Msg ) { ServerMessageCallback( Msg ); } );
    m_ServerConnection->Post( SanguisNet::Message::FromString( "", SanguisNet::MessageHeader::ID_GAME_PLAYER_LIST ) );
}

void
GameScene::Apply( )
{
    auto* UserInput = Engine::GetEngine( )->GetUserInputHandle( );

    if ( !m_CameraZoomLerp.HasReachedEnd( ) )
    {
        const auto FOV              = m_CameraZoomLerp.Update( Engine::GetEngine( )->GetDeltaSecond( ) );
        auto&      CameraController = m_CharController->GetCameraController( );
        CameraController->GetCamera( )->SetCameraPerspectiveFOV( FOV );

        m_Reticle->GetSprite( )->SetScale( glm::vec3 { 30 / FOV } );
    }

    // Zoom
    if ( UserInput->GetSecondaryKey( ) )
    {
        if ( !m_IsViewZooming )
        {
            auto& CameraController = m_CharController->GetCameraController( );
            CameraController->SetViewSensitivity( 0.4 );
            m_CameraZoomLerp.Swap( );

            m_IsViewZooming = true;
        }
    } else
    {
        if ( m_IsViewZooming )
        {
            auto& CameraController = m_CharController->GetCameraController( );
            CameraController->SetViewSensitivity( 1 );
            m_CameraZoomLerp.Swap( );

            m_IsViewZooming = false;
        }
    }

    auto*      AudioEngine   = Engine::GetEngine( )->GetAudioEngine( );
    auto*      CameraPtr     = (PureConceptPerspectiveCamera*) *m_CharController;
    const auto CameraLookRay = RayCast::CalculateRay( CameraPtr, 100 );

    AudioEngine->SetListenerPosition( CameraLookRay.RayOrigin, -CameraLookRay.UnitDirection );

    // Fire
    if ( UserInput->GetPrimaryKey( ).isPressed )
    {
        if ( UserInput->IsCursorLocked( ) )
        {
            Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( m_GunFireAudioHandle );

            m_ServerConnection->Post( SanguisNet::Message::FromStruct( CameraLookRay, SanguisNet::MessageHeader::ID_GAME_GUN_FIRE ) );

            auto CastResult = RayCast::Cast( CameraLookRay );
            spdlog::info( "CastResult: {}-{}:{}",
                          CastResult.HitDistance,
                          CastResult.HitPosition,
                          CastResult
                              ? CastResult.HitUserData
                                  ? CastResult.HitUserData->GetRuntimeName( )
                                  : "NoName"
                              : "Nan" );

            if ( CastResult.HitUserData != nullptr )
            {
                if ( auto* RB = CastResult.HitUserData->TryCast<RigidBody>( ); RB != nullptr )
                {
                    auto PRB = RB->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                    if ( PRB != nullptr )
                    {
                        physx::PxRigidBodyExt::addForceAtPos( *PRB, reinterpret_cast<physx::PxVec3&>( RayCast::CalculateRay( *m_CharController ).UnitDirection ) * 1000, *(physx::PxVec3*) &CastResult.HitPosition );
                    }
                } else if ( auto* PEUW = CastResult.HitUserData->TryCast<PostEntityUpdateWrapper>( ); PEUW != nullptr )
                {
                    const auto& RTName = PEUW->GetRuntimeName( );
                    if ( RTName.ends_with( "_RemoteController" ) )
                    {
                        const auto Name  = RTName.substr( 0, RTName.find( "_RemoteController" ) );
                        const auto Index = std::distance( m_PlayerStats.begin( ), std::find_if( m_PlayerStats.begin( ), m_PlayerStats.end( ), [ &Name ]( const auto& PlayerStat ) { return PlayerStat.Name == Name; } ) );

                        DoDamage( Index, 20 );
                    }
                }
            }
        }
    }
}

void
GameScene::ServerMessageCallback( SanguisNet::Message& Msg )
{
    switch ( Msg.header.id )
    {
    case SanguisNet::MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES: {
        const auto PlayerData = SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_PLAYER_STAT> { }( Msg );
        if ( m_PlayerControllers.size( ) <= PlayerData.PlayerIndex )
        {
            spdlog::info( "Skipping ID_GAME_UPDATE_PLAYER_COORDINATES, data corrupted" );
            break;
        }
        auto ENO = Msg.ToStruct<PostEntityUpdateWrapper::EntityNetOrientation>( );
        // spdlog::info( "Player: {} move to {} with velocity: {}", PlayerData.PlayerIndex, ENO.FootPosition, ENO.Velocity );
        m_PlayerControllers[ PlayerData.PlayerIndex ]->SetNetOrientation( ENO );
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_STAT: {
        const auto PlayerData                   = SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_PLAYER_STAT> { }( Msg );
        m_PlayerStats[ PlayerData.PlayerIndex ] = PlayerStats::FromString( Msg.ToString( ) );
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE: {
        const auto DealerData = SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE> { }( Msg );

        std::string_view Data        = { (char*) DealerData.Data.data( ), DealerData.Data.size( ) };
        int              PlayerIndex = 0;
        int              Damage      = 0;
        std::from_chars( Data.data( ), Data.data( ) + Data.find( '\0' ), PlayerIndex );
        std::from_chars( Data.data( ) + Data.find( '\0' ) + 1, Data.data( ) + Data.size( ), Damage );
        spdlog::info( "Player: {}, Damage received: {}", PlayerIndex, Damage );
        m_PlayerStats[ PlayerIndex ].Health -= Damage;
        if ( m_PlayerStats[ PlayerIndex ].Name == m_UserName )
            m_HealthText->GetText( ) = std::to_string( m_PlayerStats[ PlayerIndex ].Health );
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_GUN_FIRE: {
        const auto FireData = SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg );
        auto       Ray      = Msg.ToStruct<RayCast>( );
        spdlog::info( "Player {} fired at {} dir {} with distance {}", m_PlayerStats[ FireData.PlayerIndex ].Name, Ray.RayToCast.RayOrigin, Ray.RayToCast.UnitDirection, Ray.RayToCast.MaxDistance );
        Engine::GetEngine( )->GetAudioEngine( )->PlayAudio3D( m_GunFireAudioHandle, Ray.RayToCast.RayOrigin );
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_LIST:
        m_PlayerStats = Msg.ToString( )
            | std::views::split( '\n' )
            | std::views::transform( []( const auto& SV ) { return PlayerStats { .Name = std::string( SV.begin( ), SV.end( ) ) }; } )
            | std::ranges::to<std::vector<PlayerStats>>( );
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this ]( ) {
            auto Lock = Engine::GetEngine( )->GetPhysicsThreadLock( );
            for ( auto& PC : m_PlayerControllers )
                PC->Destroy( );
            m_PlayerControllers = m_PlayerStats
                | std::views::transform( [ this ]( const PlayerStats& PS ) -> std::shared_ptr<PostEntityUpdateWrapper> {
                                      if ( PS.Name.empty( ) ) return nullptr;
                                      bool IsMainPlayer = PS.Name == m_UserName;

                                      std::shared_ptr<PhyControllerEntity> PCE;

                                      if ( IsMainPlayer )
                                      {
                                          PCE = m_CharController;
                                      } else
                                      {
                                          PCE = PureConcept::CreateConcept<PhyControllerEntity>( );
                                          PCE->CreateController( { 0, 100, 0 }, 1.f, 0.3f, m_DefaultPhyMaterial );
                                      }

                                      auto PEUW = AddConcept<PostEntityUpdateWrapper>( PCE, m_ServerConnection, IsMainPlayer ).Get( );
                                      PEUW->SetRuntimeName( PS.Name + "_RemoteController" );
                                      PCE->SetUserData( dynamic_cast<PureConcept*>( PEUW.get( ) ) );
                                      return std::move( PEUW );
                                  } )
                | std::ranges::to<std::vector<std::shared_ptr<PostEntityUpdateWrapper>>>( );
            m_Reticle->MoveToLastAsSubConcept( );
        } );
    }
}

void
GameScene::DoDamage( int PlayerIndex, int Damage )
{
    if ( PlayerIndex >= m_PlayerStats.size( ) ) return;
    m_ServerConnection->Post( SanguisNet::Message::FromString( std::to_string( PlayerIndex ) + '\0' + std::to_string( Damage ), SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE ) );
}
