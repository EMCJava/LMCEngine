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
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Material/Material.hpp>
#include <Engine/Core/Graphic/Texture/Texture.hpp>
#include <Engine/Core/Physics/Collider/ColliderMesh.hpp>
#include <Engine/Core/Physics/RigidBody/RigidMesh.hpp>
#include <Engine/Core/Physics/Controller/PhyController.hpp>

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

DEFINE_CONCEPT_DS_MA_SE( GameManager )
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
            m_CanvasRenderables.ForEachShared( [ &Location ]( std::shared_ptr<ConceptRenderable>& Renderable ) {
                Renderable->SetShaderUniform( "lightPos", Location );
            } );
        }
    }

protected:
    FloatTy                         m_AccumulatedTime { 0.0f };
    std::shared_ptr<Canvas>         m_EffectiveCanvas;
    std::shared_ptr<RenderableMesh> m_LightMesh;

    ConceptSetCacheShared<ConceptRenderable> m_CanvasRenderables;

    OrientationMatrix m_LightOrientation;
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

        m_MainCamera->SetCameraPosition( glm::vec3( -22.228, 14.06, 10.178 ), false );
        m_MainCamera->SetCameraPrincipalAxes( -25.25, -29 );
    }

    {
        const auto& PerspectiveCanvas = AddConcept<Canvas>( ).Get( );
        PerspectiveCanvas->SetRuntimeName( "Perspective Canvas" );
        PerspectiveCanvas->SetCanvasCamera( m_MainCamera );

        {
            {
                auto LightMesh = CreateConcept<ConceptMesh>( "Assets/Model/red_cube.glb" );

                auto LightRenderable = PerspectiveCanvas->AddConcept<RenderableMesh>( LightMesh ).Get( );
                LightRenderable->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );
                AddConcept<LightRotate>( PerspectiveCanvas, LightRenderable );
            }

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

                if ( false )
                {
                    auto CottageMesh = PureConcept::CreateConcept<ConceptMesh>( "Assets/Model/cottage_obj.obj", eFeatureDefault | eFeatureUV0 );

                    auto RM = PerspectiveCanvas->AddConcept<RigidMesh>( CottageMesh, PhyMaterial, true ).Get( );

                    auto CottageRenderable = RM->GetRenderable( );
                    RenderableShaderSetup( CottageRenderable, "DefaultTexturePhongShader" );

                    auto CottageMaterial = std::make_shared<Material>( );
                    CottageMaterial->ColorTexture.LoadTexture( "Assets/Model/Texture/cottage_textures/cottage_diffuse.png" );
                    CottageRenderable->TryCast<RenderableMesh>( )->SetMaterial( std::move( CottageMaterial ) );

                    CottageRenderable->SetRuntimeName( "Cottage" );
                }

                {
                    std::shared_ptr<RigidMesh> RM = PerspectiveCanvas->AddConcept<RigidMesh>( "Assets/Model/low_poly_room.glb", PhyMaterial, true, []( auto& SubMeshSpan ) {
                        if ( SubMeshSpan.SubMeshName.find( "Wall" ) != std::string::npos )
                            return ColliderSerializerGroupMesh::ColliderType::eTriangle;
                        else
                            return ColliderSerializerGroupMesh::ColliderType::eConvex;
                    } );
                    RenderableShaderSetup( RM->GetRenderable( ), "DefaultPhongShader" );
                }

                {
                    auto Mesh             = CreateConcept<ConceptMesh>( "Assets/Model/red_cube.glb" );
                    auto MeshColliderData = std::make_shared<ColliderSerializerGroupMesh>( Mesh );

                    for ( int i = 0; i < 10; ++i )
                    {
                        auto  RM             = PerspectiveCanvas->AddConcept<RigidMesh>( Mesh, CreateConcept<ColliderMesh>( MeshColliderData, PhyMaterial ) ).Get( );
                        auto& RenderableMesh = RM->GetRenderable( );
                        RenderableShaderSetup( RenderableMesh, "DefaultPhongShader" );

                        auto* RBH = RM->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                        REQUIRED_IF( RBH != nullptr )
                        {
                            RBH->setGlobalPose( physx::PxTransform { physx::PxVec3( 0, 15 + i * 3, 0 ) } );
                            RBH->setAngularVelocity( { (float) i, (float) 20, (float) -10 }, true );
                        }
                    }
                }

                // Controller
                {
                    m_CharController = std::make_shared<PhyController>( );
                    auto Lock        = Engine::GetEngine( )->GetPhysicsThreadLock( );
                    m_CharController->CreateController( { 0, 100, 0 }, 1.f, 0.3f, PhyMaterial );
                }
            }
        }
    }

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    m_CharController->MoveRel( { 0, -9.81f, 0 }, Engine::GetEngine( )->GetDeltaSecond( ) );
    const auto Pos = m_CharController->GetFootPosition( );

    spdlog::info( "Foot Position: {},{},{}", Pos.x, Pos.y, Pos.z );
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
            m_WandEditorScene = AddConcept<WandEditorScene>( ).Get( );
        }
    }
}