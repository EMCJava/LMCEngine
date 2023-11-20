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
    LightRotate( std::shared_ptr<ConceptRenderable> Mesh, std::shared_ptr<RenderableMesh> LightMesh )
        : m_Mesh( std::move( Mesh ) )
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
        m_Mesh->SetShaderUniform( "lightPos", Location );
        m_LightOrientation.SetCoordinate( Location, true );

        m_LightMesh->SetShaderUniform( "modelMatrix", m_LightOrientation.GetModelMatrix( ) );
    }

protected:
    FloatTy                            m_AccumulatedTime { 0.0f };
    std::shared_ptr<ConceptRenderable> m_Mesh;
    std::shared_ptr<RenderableMesh>    m_LightMesh;

    Orientation m_LightOrientation;
};
DEFINE_CONCEPT_DS( LightRotate )

class RigidBody : public ConceptApplicable
    , protected Orientation
{
    DECLARE_CONCEPT( RigidBody, ConceptApplicable )
public:
    RigidBody( )
    {
        // For collider or renderable etc.
        SetSearchThrough( true );
    }

    void
    Apply( ) override
    {
        if ( OrientationChanged )
        {
            if ( m_Renderable != nullptr )
            {
                m_Renderable->SetShaderUniform( "modelMatrix", GetModelMatrix( ) );
            }
            OrientationChanged = false;
        }
    }

    [[nodiscard]] const auto&
    GetRenderable( ) const
    {
        return m_Renderable;
    }

    Orientation&
    GetOrientation( )
    {
        OrientationChanged = true;
        return *static_cast<Orientation*>( this );
    }

protected:
    bool OrientationChanged = true;

    // FIXME: Consider making these two unique_ptr, for 1 to 1 relationship
    std::shared_ptr<ConceptRenderable> m_Renderable;
    std::shared_ptr<PureConceptCollider>          m_Collider;
};
DEFINE_CONCEPT_DS( RigidBody )

/*
 *
 * Contain renderable mesh and collider
 *
 * */
class RigidMesh : public RigidBody
{
    DECLARE_CONCEPT( RigidMesh, RigidBody )

public:
    template <typename Mapping = void*>
    RigidMesh( const std::string& MeshPathStr, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
    {
        REQUIRED( !MeshPathStr.empty( ), throw std::runtime_error( "Mesh path cannot be empty" ); )

        SerializerModel Serializer;

        auto Mesh = CreateConcept<ConceptMesh>( );
        Serializer.ToMesh( MeshPathStr, Mesh.get( ) );

        SetMesh( Mesh );

        REQUIRED( PhyEngine != nullptr && Material != nullptr, throw std::runtime_error( "Physx engine and material cannot be nullptr" ); )
        SetCollider( CreateConcept<ColliderMesh>( Mesh, PhyEngine, Material, Static, ColliderMapping ) );
    }

    RigidMesh( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<PureConceptCollider> C )
    {
        SetMesh( std::move( Mesh ) );
        SetCollider( std::move( C ) );
    }

    void
    SetMesh( std::shared_ptr<ConceptMesh> Mesh )
    {
        m_Renderable = AddConcept<RenderableMesh>( std::move( Mesh ) );
    }

    void
    SetCollider( std::shared_ptr<PureConceptCollider> C )
    {
        if ( m_Collider != nullptr ) m_Collider->Destroy( );
        REQUIRED( GetOwnership( m_Collider = std::move( C ) ) )
    }

    [[nodiscard]] const auto&
    GetCollider( ) const noexcept { return m_Collider; }
};
DEFINE_CONCEPT_DS( RigidMesh )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    constexpr std::pair<int, int> WindowSize = { 1920, 1080 };
    Engine::GetEngine( )->SetLogicalMainWindowViewPortDimensions( WindowSize );

    {
        m_MainCamera = AddConcept<PureConceptPerspectiveCamera>( );
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
            auto StaticMesh = AddConcept<ConceptMesh>( );
            StaticMesh->DetachFromOwner( );

            {
                SerializerModel TestModel;
                TestModel.SetFilePath( "Assets/Model/red_cube.glb" );
                TestModel.ToMesh( StaticMesh.get( ) );
            }

            auto LightMesh = PerspectiveCanvas->AddConcept<RenderableMesh>( StaticMesh );
            LightMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );

            AddConcept<FirstPersonCameraController>( m_MainCamera );

            if ( true )
            {
                m_PhyEngine = std::make_shared<PhysicsEngine>( );

                physx::PxMaterial*    Material    = ( *m_PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *m_PhyEngine, physx::PxPlane( 0, 1, 0, 3 ), *Material );
                m_PhyEngine->GetScene( )->addActor( *groundPlane );

                {
                    auto  RM             = AddConcept<RigidMesh>( "Assets/Model/low_poly_room.glb", m_PhyEngine.get( ), Material, true, []( auto& SubMeshSpan ) {
                        if ( SubMeshSpan.SubMeshName.find( "Wall" ) != std::string::npos )
                            return ColliderSerializerGroupMesh::ColliderType::eTriangle;
                        else
                            return ColliderSerializerGroupMesh::ColliderType::eConvex;
                    } );
                    auto& RenderableMesh = RM->GetRenderable( );
                    RenderableMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
                    RenderableMesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
                    RenderableMesh->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
                    RenderableMesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
                    AddConcept<LightRotate>( RenderableMesh, LightMesh );
                }

                {
                    SerializerModel Serializer;
                    auto            Mesh = CreateConcept<ConceptMesh>( );
                    Serializer.ToMesh( "Assets/Model/red_cube.glb", Mesh.get( ) );
                    auto MeshColliderData = std::make_shared<ColliderSerializerGroupMesh>( StaticMesh, m_PhyEngine.get( ) );

                    for ( int i = 0; i < 10; ++i )
                    {
                        auto  RM             = AddConcept<RigidMesh>( Mesh, CreateConcept<ColliderMesh>( MeshColliderData, m_PhyEngine.get( ), Material ) );
                        auto& RenderableMesh = RM->GetRenderable( );
                        RenderableMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
                        RenderableMesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
                        RenderableMesh->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
                        RenderableMesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );

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