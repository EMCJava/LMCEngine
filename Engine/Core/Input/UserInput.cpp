//
// Created by Lo Yu Sum on 2/7/2023.
//

#include "UserInput.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

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

    std::swap( m_NewKeyPresses.first, m_NewKeyPresses.second );
    m_NewKeyPresses.second.clear( );

    std::swap( m_NewCharPresses.first, m_NewCharPresses.second );
    m_NewCharPresses.second.clear( );

    UpdateMouse( );
}

void
UserInput::UpdateMouse( )
{
    double X, Y;
    glfwGetCursorPos( m_EventWindow, &X, &Y );

    m_CursorDeltaPosition.first  = X - m_CursorPosition.first;
    m_CursorDeltaPosition.second = Y - m_CursorPosition.second;

    m_CursorPosition.first  = X;
    m_CursorPosition.second = Y;
}

void
UserInput::RegisterKeyUpdate( bool pressed, KeyIDTY id )
{
    auto& key = m_OtherKeysStates[ id ];

    if ( pressed )
    {
        m_NewKeyPresses.second.push_back( id );
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

KeyState&
UserInput::GetPrimaryKey( )
{
    return GetKeyState( GLFW_MOUSE_BUTTON_LEFT );
}
KeyState&
UserInput::GetSecondaryKey( )
{
    return GetKeyState( GLFW_MOUSE_BUTTON_RIGHT );
}
KeyState&
UserInput::GetFunctionKey( )
{
    return GetKeyState( GLFW_MOUSE_BUTTON_MIDDLE );
}

void
UserInput::EnableRowCursorMotion( bool Enable )
{
    m_EnableRowCursorMotion = Enable;
    REQUIRED_IF( glfwRawMouseMotionSupported( ) )
    {
        glfwSetInputMode( m_EventWindow, GLFW_RAW_MOUSE_MOTION, Enable );
    }
}

void
UserInput::LockCursor( bool Lock )
{
    if ( ( m_IsCursorLocked = Lock ) )
    {
        ( reinterpret_cast<decltype( &glfwSetInputMode )>( glfwSetInputModePtr ) )( m_EventWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        EnableRowCursorMotion( m_EnableRowCursorMotion );
    } else
    {
        ( reinterpret_cast<decltype( &glfwSetInputMode )>( glfwSetInputModePtr ) )( m_EventWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
    }

    // Reset CursorDeltaPosition
    UpdateMouse( );
    UpdateMouse( );
}

void
UserInput::StoreGLFWContext( )
{
    glfwSetInputModePtr = reinterpret_cast<void ( * )( )>( glfwSetInputMode );
}

void
UserInput::PushCharInput( uint32_t CodePoint )
{
    m_NewCharPresses.second.push_back( CodePoint );
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
    static constexpr auto KeyRepeatFrame    = 64;
    static constexpr auto KeyRepeatInterval = 16;

    isRepeat        = isDown && lastDownFrame <= frame - KeyRepeatFrame && ( frame - lastDownFrame ) % KeyRepeatInterval == 0;
    isPressed       = isDown && lastDownFrame == frame - 1;
    isDoublePressed = lastDoublePressedFrame == frame - 1;

    return *this;
}
