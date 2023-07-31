//
// Created by Lo Yu Sum on 2/7/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <chrono>
#include <cstdint>
#include <map>

struct GLFWwindow;

struct KeyState {
    bool     isDown                 = false;
    bool     isPressed              = false;
    bool     isDoublePressed        = false;
    uint32_t lastDownFrame          = 0;
    uint32_t lastDoublePressedFrame = 0;

    bool                                               timeInitialized = false;
    std::chrono::time_point<std::chrono::steady_clock> lastPressedTime { };

    void SetPress( uint32_t frame );

    KeyState& Update( uint32_t frame );
};

using KeyIDTY = int;
class UserInput
{
public:
    explicit UserInput( GLFWwindow* window = nullptr );

    inline void SetEventWindow( GLFWwindow* window ) { m_EventWindow = window; }
    inline void SetMouseSensitivity( float mouseSensitivity ) { m_MouseSensitivity = mouseSensitivity; }

    void      RegisterKeyUpdate( bool pressed, KeyIDTY id );
    KeyState& GetKeyState( KeyIDTY id );

    void Update( );

    [[nodiscard]] inline KeyState GetPrimaryKey( ) const { return m_PrimaryKey; }
    [[nodiscard]] inline KeyState GetSecondaryKey( ) const { return m_SecondaryKey; }
    [[nodiscard]] inline KeyState GetFunctionKey( ) const { return m_FunctionKey; }

    [[nodiscard]] inline FloatTy  GetMouseSensitivity( ) const { return m_MouseSensitivity; }
    [[nodiscard]] inline uint32_t GetFrameCount( ) const { return m_FrameCount; }
    [[nodiscard]] inline int32_t  GetKeyPressCount( ) const { return m_KeyPressingCount; }

private:
    uint32_t    m_FrameCount = 0;
    GLFWwindow* m_EventWindow { };

    KeyState m_PrimaryKey;
    KeyState m_SecondaryKey;
    KeyState m_FunctionKey;

    std::map<KeyIDTY, KeyState> m_OtherKeysStates { };

    FloatTy m_MouseSensitivity { 0.5f };

    int32_t m_KeyPressingCount { };

    void UpdateMouse( );
};