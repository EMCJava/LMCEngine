//
// Created by Lo Yu Sum on 2/7/2023.
//

#include "UserInput.hpp"

#include <GLFW/glfw3.h>

UserInput::UserInput( GLFWwindow* window )
    : m_EventWindow( window )
{
}

void
UserInput::Update( )
{
    if ( !m_EventWindow ) return;
    ++m_FrameCount;

    UpdateMouse( );
}

void
UserInput::UpdateMouse( )
{
    m_PrimaryKey.isPressed = false;
    if ( glfwGetMouseButton( m_EventWindow, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS )
        m_PrimaryKey.SetPress( m_FrameCount );
    else
        m_PrimaryKey.isDown = false;

    m_SecondaryKey.isPressed = false;
    if ( glfwGetMouseButton( m_EventWindow, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS )
        m_SecondaryKey.SetPress( m_FrameCount );
    else
        m_SecondaryKey.isDown = false;

    m_FunctionKey.isPressed = false;
    if ( glfwGetMouseButton( m_EventWindow, GLFW_MOUSE_BUTTON_MIDDLE ) == GLFW_PRESS )
        m_FunctionKey.SetPress( m_FrameCount );
    else
        m_FunctionKey.isDown = false;

    double X, Y;
    glfwGetCursorPos( m_EventWindow, &X, &Y );
    m_CursorPosition.first  = X;
    m_CursorPosition.second = Y;
}

void
UserInput::RegisterKeyUpdate( bool pressed, KeyIDTY id )
{
    auto& key = m_OtherKeysStates[ id ];

    if ( pressed )
    {
        m_KeyPressingCount += 1;
        key.SetPress( m_FrameCount );
    } else
    {
        m_KeyPressingCount -= 1;
        key.isDown = false;
    }
}

KeyState&
UserInput::GetKeyState( KeyIDTY id )
{
    return m_OtherKeysStates[ id ].Update( m_FrameCount );
}

void
KeyState::SetPress( uint32_t frame )
{
    const auto currentTime = std::chrono::steady_clock::now( );

    // New press
    if ( !isDown )
    {
        const auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>( currentTime - lastPressedTime ).count( );
        if ( timeInitialized && !isPressed && timeDiff < 200 )
        {
            isDoublePressed        = true;
            lastDoublePressedFrame = frame;
        }

        isPressed = true;
    } else
    {
        isPressed = false;
    }

    isDown = true;

    lastDownFrame   = frame;
    lastPressedTime = currentTime;
    timeInitialized = true;
}

KeyState&
KeyState::Update( uint32_t frame )
{
    // key.isPressed might not be most up-to-date
    isPressed       = isDown && lastDownFrame == frame - 1;
    isDoublePressed = lastDoublePressedFrame == frame - 1;

    return *this;
}
