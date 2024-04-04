//
// Created by EMCJava on 4/3/2024.
//

#include "GameScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Camera/FirstPersonCameraController.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshCluster.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>

#include <Engine/Core/Scene/Entity/EntityRenderable.hpp>

#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Input/UserInput.hpp>

#include <Engine/Core/Physics/Controller/PhyControllerEntityPlayer.hpp>
#include <Engine/Core/Physics/RigidBody/RigidMesh.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>

#include <PxPhysicsAPI.h>

#include <ranges>

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

GameScene::GameScene( std::shared_ptr<SanguisNet::ClientGroupParticipant> Connection )
    : m_ServerConnection( std::move( Connection ) )
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

                physx::PxMaterial*    PhyMaterial = ( *PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *PhyEngine, physx::PxPlane( 0, 1, 0, 0 ), *PhyMaterial );
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
                    std::shared_ptr<RigidMesh> RM = PerspectiveCanvas->AddConcept<RigidMesh>( CM, CreateConcept<ColliderMesh>( CM, PhyMaterial, true, []( auto& SubMeshSpan ) {
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
                    m_CharController->CreateController( { 0, 100, 0 }, 1.f, 0.3f, PhyMaterial );
                }
            }
        }
    }
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

    // Fire
    if ( UserInput->GetPrimaryKey( ).isPressed )
    {
        if ( UserInput->IsCursorLocked( ) )
        {
            auto*      CameraPtr = (PureConceptPerspectiveCamera*) *m_CharController;
            const auto Ray       = RayCast::CalculateRay( CameraPtr, 100 );
            m_ServerConnection->Post( SanguisNet::Message::FromStruct( Ray, SanguisNet::MessageHeader::ID_GAME_GUN_FIRE ) );

            auto CastResult = RayCast::Cast( Ray );
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
                auto RB = CastResult.HitUserData->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                if ( RB != nullptr )
                {
                    auto Ray = RayCast::CalculateRay( *m_CharController );
                    physx::PxRigidBodyExt::addForceAtPos( *RB, ( *(physx::PxVec3*) &Ray.UnitDirection ) * 1000, *(physx::PxVec3*) &CastResult.HitPosition );
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
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_GUN_FIRE: {
        const auto FireData = SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg );
        auto       Ray      = Msg.ToStruct<RayCast>( );
        spdlog::info( "Player {} fired at {} dir {} with distance {}", m_PlayerStats[ FireData.PlayerIndex ].Name, Ray.RayToCast.RayOrigin, Ray.RayToCast.UnitDirection, Ray.RayToCast.MaxDistance );
        break;
    }
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_LIST: {
        m_PlayerStats = Msg.ToString( )
            | std::views::split( '\n' )
            | std::views::transform( []( const auto& SV ) { return PlayerStats { .Name = std::string( SV.begin( ), SV.end( ) ) }; } )
            | std::ranges::to<std::vector<PlayerStats>>( );
    }
    }
}

void
GameScene::DoDamage( int PlayerIndex, int Damage )
{
    m_PlayerStats[PlayerIndex].Health -= Damage;
    m_ServerConnection->Post( SanguisNet::Message::FromString( std::to_string( PlayerIndex ) + '\0' + std::to_string( Damage ), SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE ) );
}
