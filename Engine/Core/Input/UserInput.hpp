//
// Created by Lo Yu Sum on 2/7/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <chrono>
#include <cstdint>
#include <map>

namespace
{
inline std::pair<double, double>
operator-( const std::pair<double, double>& A, const std::pair<double, double>& B )
{
    return { A.first - B.first, A.second - B.second };
}
}   // namespace

struct GLFWwindow;

struct KeyState {
    bool     isDown                 = false;
    bool     isRepeat               = false;
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
    inline void SetCursorSensitivity( float mouseSensitivity ) { m_CursorSensitivity = mouseSensitivity; }
    inline void SetCursorTopLeftPosition( const std::pair<int, int>& CursorTopLeftPosition ) { m_CursorTopLeftPosition = CursorTopLeftPosition; }

    void      RegisterKeyUpdate( bool pressed, KeyIDTY id );
    KeyState& GetKeyState( KeyIDTY id );

    void Update( );

    [[nodiscard]] inline KeyState GetPrimaryKey( ) const { return m_PrimaryKey; }
    [[nodiscard]] inline KeyState GetSecondaryKey( ) const { return m_SecondaryKey; }
    [[nodiscard]] inline KeyState GetFunctionKey( ) const { return m_FunctionKey; }

    [[nodiscard]] inline FloatTy             GetCursorSensitivity( ) const { return m_CursorSensitivity; }
    [[nodiscard]] inline std::pair<int, int> GetCursorPosition( ) const { return m_CursorPosition - m_CursorTopLeftPosition; }
    [[nodiscard]] inline uint32_t            GetFrameCount( ) const { return m_FrameCount; }
    [[nodiscard]] inline int32_t             GetKeyPressCount( ) const { return m_KeyPressingCount; }

private:
    uint32_t    m_FrameCount = 0;
    GLFWwindow* m_EventWindow { };

    KeyState            m_PrimaryKey;
    KeyState            m_SecondaryKey;
    KeyState            m_FunctionKey;
    std::pair<int, int> m_CursorTopLeftPosition;
    std::pair<int, int> m_CursorPosition;

    std::map<KeyIDTY, KeyState> m_OtherKeysStates { };

    FloatTy m_CursorSensitivity { 0.5f };

    int32_t m_KeyPressingCount { };

    void UpdateMouse( );
};