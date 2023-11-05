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

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, ConceptApplicable, TestInvokable )

class MeshRotate : public ConceptApplicable
{
    DECLARE_CONCEPT( MeshRotate, ConceptApplicable )
public:
    MeshRotate( std::shared_ptr<ConceptMesh> Mesh )
        : m_Mesh( Mesh )
    { }

    void
    Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_AccumulatedTime += DeltaTime;

        PureConceptPerspectiveCamera* Camera;
        if ( m_Mesh->GetActiveCamera( )->TryCast( Camera ) )
        {
            m_CameraPosition = glm::vec3( sin( m_AccumulatedTime ) * 10.0f, 10, cos( m_AccumulatedTime ) * 10.0f );
            m_CameraFacing   = glm::normalize( -Camera->GetCameraPosition( ) );
            m_Mesh->SetShaderUniform( "lightPos", glm::vec3( cos( m_AccumulatedTime * 2 ) * -10.0f, 10, sin( m_AccumulatedTime * 2 ) * -10.0f ) + glm::vec3( 0, 2, 0 ) );
            Camera->SetCameraPosition( m_CameraPosition, false );
            Camera->SetCameraFacing( m_CameraFacing );
        }
    }

protected:
    glm::vec3 m_CameraPosition;
    glm::vec3 m_CameraFacing;

    FloatTy                      m_AccumulatedTime { 0.0f };
    std::shared_ptr<ConceptMesh> m_Mesh;

    ENABLE_IMGUI( MeshRotate )
};
DEFINE_CONCEPT_DS( MeshRotate )
DEFINE_SIMPLE_IMGUI_TYPE( MeshRotate, m_CameraPosition, m_CameraFacing )

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

        SerializerModel TestModel;
        TestModel.SetFilePath( "Assets/Model/low_poly_room.glb" );

        auto Mesh = PerspectiveCanvas->AddConcept<ConceptMesh>( );
        Mesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );

        Mesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
        Mesh->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
        Mesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );

        TestModel.LoadModel( Mesh.get( ) );
        // AddConcept<MeshRotate>( Mesh );
        AddConcept<CameraController>( m_MainCamera );
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