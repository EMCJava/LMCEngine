//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"

#include "WandEditorScene.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <assimp/scene.h>

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
    LightRotate( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<ConceptMesh> LightMesh )
        : m_Mesh( std::move( Mesh ) )
        , m_LightMesh( std::move( LightMesh ) )
    {
        m_LightMesh->SetScale( 0.08, 0.08, 0.08, true );
    }

    void
    Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_AccumulatedTime += DeltaTime;

        const auto Location = glm::vec3( cos( m_AccumulatedTime * 2 ) * 5.0f, 2, sin( m_AccumulatedTime * 2 ) * 5.0f );
        m_Mesh->SetShaderUniform( "lightPos", Location );
        m_LightMesh->SetCoordinate( Location, true );
    }

protected:
    FloatTy                      m_AccumulatedTime { 0.0f };
    std::shared_ptr<ConceptMesh> m_Mesh;
    std::shared_ptr<ConceptMesh> m_LightMesh;
};
DEFINE_CONCEPT_DS( LightRotate )

class CameraController : public ConceptApplicable
{
    DECLARE_CONCEPT( CameraController, ConceptApplicable )

public:
    explicit CameraController( std::shared_ptr<PureConceptPerspectiveCamera> Camera )
        : m_Camera( std::move( Camera ) )
    {
        Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( true );
    }

    void
    Apply( ) override
    {
        const auto    DeltaTime      = Engine::GetEngine( )->GetDeltaSecond( );
        const FloatTy DeltaChange    = DeltaTime * m_ViewControlSensitivity;
        auto          CameraPosition = m_Camera->GetCameraPosition( );

        int FrontMovement = 0;
        int RightMovement = 0;

        auto* UserInputHandle = Engine::GetEngine( )->GetUserInputHandle( );

        if ( UserInputHandle->GetKeyState( GLFW_KEY_W ).isDown ) FrontMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_S ).isDown ) FrontMovement -= 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_D ).isDown ) RightMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_A ).isDown ) RightMovement -= 1;

        if ( FrontMovement != 0 ) CameraPosition += m_Camera->GetCameraFacing( ) * ( FrontMovement * DeltaChange );
        if ( RightMovement != 0 ) CameraPosition += m_Camera->GetCameraRightVector( ) * ( RightMovement * DeltaChange );
        if ( FrontMovement || RightMovement ) m_Camera->SetCameraPosition( CameraPosition );

        if ( UserInputHandle->GetKeyState( GLFW_KEY_ESCAPE ).isPressed )
        {
            Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( ( m_MouseLocked = !m_MouseLocked ) );
        }

        if ( m_MouseLocked )
        {
            const auto Delta = UserInputHandle->GetCursorDeltaPosition( );
            // spdlog::info( "Delta movement: {},{}", Delta.first, Delta.second );
            m_Camera->AlterCameraPrincipalAxes( Delta.first * 0.05F, -Delta.second * 0.05F );
        }
    }

protected:
    std::shared_ptr<PureConceptPerspectiveCamera> m_Camera;
    FloatTy                                       m_ViewControlSensitivity { 1.0f };

    bool m_MouseLocked { true };

    ENABLE_IMGUI( CameraController )
};
DEFINE_CONCEPT( CameraController )
DEFINE_SIMPLE_IMGUI_TYPE( CameraController, m_ViewControlSensitivity )

CameraController::~CameraController( )
{
    Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( false );
}

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
            SerializerModel TestModel;

            TestModel.SetFilePath( "Assets/Model/low_poly_room.glb" );
            auto Mesh = PerspectiveCanvas->AddConcept<ConceptMesh>( );
            Mesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
            Mesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
            Mesh->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
            Mesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
            TestModel.LoadModel( Mesh.get( ) );


            TestModel.SetFilePath( "Assets/Model/red_cube.glb" );
            auto LightMesh = PerspectiveCanvas->AddConcept<ConceptMesh>( );
            LightMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );
            TestModel.LoadModel( LightMesh.get( ) );

            AddConcept<LightRotate>( Mesh, LightMesh );
            AddConcept<CameraController>( m_MainCamera );
        }
    }

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
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