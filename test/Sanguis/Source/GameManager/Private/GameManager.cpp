//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"

#include "WandEditorScene.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Camera/FirstPersonCameraController.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>
#include <Engine/Core/Physics/Collider/Serializer/ColliderSerializerGroupMesh.hpp>
#include <Engine/Core/Physics/Collider/ColliderMesh.hpp>
#include <Engine/Core/Physics/RigidBody/RigidMesh.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <PxPhysicsAPI.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <ranges>
#include <utility>

DEFINE_CONCEPT_MA_SE( GameManager )
GameManager::~GameManager( )
{
    // FIXME: Tmp fix to avoid PhyEng destructing before all concepts
    RemoveConcepts<PureConcept>( );
}
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, ConceptApplicable, TestInvokable )

class LightRotate : public ConceptApplicable
{
    DECLARE_CONCEPT( LightRotate, ConceptApplicable )
public:
    LightRotate( std::shared_ptr<Canvas> EffectiveCanvas, std::shared_ptr<RenderableMesh> LightMesh )
        : m_EffectiveCanvas( std::move( EffectiveCanvas ) )
        , m_LightMesh( std::move( LightMesh ) )
    {
        m_LightOrientation.SetScale( 0.08, 0.08, 0.08 );
    }

    void
    Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_AccumulatedTime += DeltaTime;

        const auto Location = glm::vec3( cos( m_AccumulatedTime * 2 ) * 5.0f, 2, sin( m_AccumulatedTime * 2 ) * 5.0f );
        m_LightOrientation.SetCoordinate( Location, true );
        m_LightMesh->SetShaderUniform( "modelMatrix", m_LightOrientation.GetModelMatrix( ) );

        REQUIRED_IF( m_EffectiveCanvas != nullptr )
        {
            m_EffectiveCanvas->GetConcepts( m_CanvasRenderables );
            m_CanvasRenderables.ForEach( [ &Location ]( std::shared_ptr<ConceptRenderable>& Renderable ) {
                Renderable->SetShaderUniform( "lightPos", Location );
            } );
        }
    }

protected:
    FloatTy                         m_AccumulatedTime { 0.0f };
    std::shared_ptr<Canvas>         m_EffectiveCanvas;
    std::shared_ptr<RenderableMesh> m_LightMesh;

    ConceptSetFetchCache<ConceptRenderable> m_CanvasRenderables;

    Orientation m_LightOrientation;
};
DEFINE_CONCEPT_DS( LightRotate )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    constexpr std::pair<int, int> WindowSize = { 1920, 1080 };
    Engine::GetEngine( )->SetLogicalMainWindowViewPortDimensions( WindowSize );

    {
        m_MainCamera = AddConcept<PureConceptPerspectiveCamera>( );
        AddConcept<FirstPersonCameraController>( m_MainCamera );

        m_MainCamera->SetRuntimeName( "Main Camera" );
        m_MainCamera->PushToCameraStack( );

        m_MainCamera->SetCameraPerspectiveFOV( 45, false );
        m_MainCamera->SetCameraPosition( glm::vec3( 0.0f, 3.0f, 3.0f ), false );
        m_MainCamera->SetCameraFacing( glm::vec3( 0.0f, -0.70710678118F, -0.70710678118F ), false );
    }

    {
        const auto& PerspectiveCanvas = AddConcept<Canvas>( );
        PerspectiveCanvas->SetRuntimeName( "Perspective Canvas" );
        PerspectiveCanvas->SetCanvasCamera( m_MainCamera );

        {

            {
                auto LightMesh = CreateConcept<ConceptMesh>( "Assets/Model/red_cube.glb" );

                auto LightRenderable = PerspectiveCanvas->AddConcept<RenderableMesh>( LightMesh );
                LightRenderable->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );
                AddConcept<LightRotate>( PerspectiveCanvas, LightRenderable );
            }

            {
                m_PhyEngine = std::make_shared<PhysicsEngine>( );

                physx::PxMaterial*    Material    = ( *m_PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *m_PhyEngine, physx::PxPlane( 0, 1, 0, 3 ), *Material );
                m_PhyEngine->GetScene( )->addActor( *groundPlane );

                const auto RenderableShaderSetup = [ this ]( auto& Renderable, const std::string& ShaderName ) {
                    Renderable->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( ShaderName ) );
                    Renderable->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
                    Renderable->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
                    Renderable->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
                };

                {
                    auto RM = PerspectiveCanvas->AddConcept<RigidMesh>( "Assets/Model/low_poly_room.glb", m_PhyEngine.get( ), Material, true, []( auto& SubMeshSpan ) {
                        if ( SubMeshSpan.SubMeshName.find( "Wall" ) != std::string::npos )
                            return ColliderSerializerGroupMesh::ColliderType::eTriangle;
                        else
                            return ColliderSerializerGroupMesh::ColliderType::eConvex;
                    } );
                    RenderableShaderSetup( RM->GetRenderable( ), "DefaultPhongShader" );
                }

                {
                    auto Mesh             = CreateConcept<ConceptMesh>( "Assets/Model/red_cube.glb" );
                    auto MeshColliderData = std::make_shared<ColliderSerializerGroupMesh>( Mesh, m_PhyEngine.get( ) );

                    for ( int i = 0; i < 10; ++i )
                    {
                        auto  RM             = PerspectiveCanvas->AddConcept<RigidMesh>( Mesh, CreateConcept<ColliderMesh>( MeshColliderData, m_PhyEngine.get( ), Material ) );
                        auto& RenderableMesh = RM->GetRenderable( );
                        RenderableShaderSetup( RenderableMesh, "DefaultPhongShader" );

                        auto* RBH = RM->GetCollider( )->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                        REQUIRED_IF( RBH != nullptr )
                        {
                            RBH->setGlobalPose( physx::PxTransform { physx::PxVec3( 0, 15 + i * 3, 0 ) } );
                            RBH->setAngularVelocity( { (float) i, (float) 20, (float) -10 }, true );
                        }
                    }
                }
            }
        }
    }

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    constexpr int DesieredFPS = 160;
    constexpr int CapFPS      = DesieredFPS * 2;

    const auto DeltaSecond = Engine::GetEngine( )->GetDeltaSecond( );

    {
        m_PhyEngine->GetScene( )->simulate( DeltaSecond );

        float         WaitTimeMilliseconds      = 500.F / DesieredFPS;
        constexpr int MaxWaitTimeMilliseconds   = 1000;
        int           TotalWaitTimeMilliseconds = 0;
        while ( !m_PhyEngine->GetScene( )->fetchResults( false ) )
        {
            const auto SleepTime = std::min( MaxWaitTimeMilliseconds, int( WaitTimeMilliseconds ) );
            TotalWaitTimeMilliseconds += SleepTime;
            spdlog::info( "Wait fetchResults for {}ms, acc: {}ms.", SleepTime, TotalWaitTimeMilliseconds );
            std::this_thread::sleep_for( std::chrono::milliseconds( SleepTime ) );
            WaitTimeMilliseconds = SleepTime * 1.1F;
        }

        const auto SleepTime = 1000.F / CapFPS - DeltaSecond * 1000;
        if ( SleepTime > TotalWaitTimeMilliseconds )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( int( SleepTime ) - TotalWaitTimeMilliseconds ) );
            spdlog::info( "Sleep for stable framerate: {}ms.", int( SleepTime ) - TotalWaitTimeMilliseconds );
        }
    }

    // retrieve array of actors that moved
    physx::PxU32     nbActiveActors;
    physx::PxActor** activeActors = m_PhyEngine->GetScene( )->getActiveActors( nbActiveActors );

    if ( nbActiveActors != 0 )
    {
        // update each render object with the new transform
        for ( physx::PxU32 i = 0; i < nbActiveActors; ++i )
        {
            auto* Data = activeActors[ i ]->userData;
            if ( Data != nullptr )
            {
                auto* ColliderMeshPtr = static_cast<PureConceptCollider*>( Data );

                auto* ColliderOwnerPtr = ColliderMeshPtr->GetOwner( );
                if ( auto RB = ColliderOwnerPtr->TryCast<RigidBody>( ); RB != nullptr )
                {
                    const auto& GP = ( (physx::PxRigidActor*) activeActors[ i ] )->getGlobalPose( );
                    RB->GetOrientation( ).SetCoordinate( GP.p.x, GP.p.y, GP.p.z );

                    static_assert( sizeof( physx::PxQuat ) == sizeof( glm::quat ) );
                    RB->GetOrientation( ).SetQuat( *( (glm::quat*) &GP.q ) );
                }
            }
        }
    }
}

void
GameManager::TestInvokable( )
{
    spdlog::info( "GameManager::TestInvokable" );

    if ( GetConcept<WandEditorScene>( ) )
    {
        if ( m_WandEditorScene != nullptr )
        {
            m_WandEditorScene->Destroy( );
        }
    } else
    {
        if ( m_WandEditorScene != nullptr )
        {
            GetOwnership( m_WandEditorScene );
        } else
        {
            m_WandEditorScene = AddConcept<WandEditorScene>( );
        }
    }
}