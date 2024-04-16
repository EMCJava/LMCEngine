//
// Created by samsa on 11/21/2023.
//

#include "FirstPersonCameraController.hpp"
#include "PureConceptPerspectiveCamera.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Input/UserInput.hpp>

#include <GLFW/glfw3.h>

DEFINE_CONCEPT( FirstPersonCameraController )
DEFINE_SIMPLE_IMGUI_TYPE( FirstPersonCameraController, m_ViewControlSensitivity )

FirstPersonCameraController::FirstPersonCameraController( std::shared_ptr<PureConceptPerspectiveCamera> Camera )
    : m_Camera( std::move( Camera ) )
{
    m_MouseLocked = true;
    Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( true );
}

FirstPersonCameraController::~FirstPersonCameraController( )
{
    m_MouseLocked = false;
    Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( false );
}

void
FirstPersonCameraController::Apply( )
{
    if ( !m_Enabled ) return;

    const auto DeltaTime      = Engine::GetEngine( )->GetDeltaSecond( );
    auto       CameraPosition = m_Camera->GetCameraPosition( );

    auto* UserInputHandle = Engine::GetEngine( )->GetUserInputHandle( );

    if ( m_FreeCamera )
    {
        int FrontMovement = 0;
        int RightMovement = 0;

        const bool Accelerated = UserInputHandle->GetKeyState( GLFW_KEY_LEFT_CONTROL ).isDown;

        if ( UserInputHandle->GetKeyState( GLFW_KEY_W ).isDown ) FrontMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_S ).isDown ) FrontMovement -= 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_D ).isDown ) RightMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_A ).isDown ) RightMovement -= 1;

        if ( Accelerated )
        {
            FrontMovement <<= 4;
            RightMovement <<= 4;
        }

        if ( FrontMovement != 0 ) CameraPosition += m_Camera->GetCameraFacing( ) * ( FrontMovement * DeltaTime );
        if ( RightMovement != 0 ) CameraPosition += m_Camera->GetCameraRightVector( ) * ( RightMovement * DeltaTime );
        if ( FrontMovement || RightMovement ) m_Camera->SetCameraPosition( CameraPosition );
    }

    if ( UserInputHandle->GetKeyState( GLFW_KEY_ESCAPE ).isPressed )
    {
        Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( ( m_MouseLocked = !m_MouseLocked ) );
    }

    if ( m_MouseLocked )
    {
        const auto Delta = UserInputHandle->GetCursorDeltaPosition( ) * m_ViewControlSensitivity;
        m_Camera->AlterCameraPrincipalAxes( Delta.first * 0.05F, -Delta.second * 0.05F );
    }
}
