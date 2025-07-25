//
// Created by Lo Yu Sum on 2/7/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <utility>
#include <cstdint>
#include <chrono>
#include <vector>
#include <map>

namespace
{
template <typename Ty>
inline std::pair<Ty, Ty>
operator-( const std::pair<Ty, Ty>& A, const std::pair<Ty, Ty>& B )
{
    return { A.first - B.first, A.second - B.second };
}
template <typename Ty, typename ScTy>
inline auto
operator*( const std::pair<Ty, Ty>& A, const ScTy& B )
{
    return std::pair { A.first * B, A.second * B };
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

    operator bool( ) const { return isDown; }
};

using KeyIDTY = int;
class UserInput
{
public:
    explicit UserInput( GLFWwindow* window = nullptr );

    inline void SetEventWindow( GLFWwindow* window ) { m_EventWindow = window; }
    inline void SetCursorSensitivity( float mouseSensitivity ) { m_CursorSensitivity = mouseSensitivity; }
    inline void SetCursorTopLeftPosition( const std::pair<int, int>& CursorTopLeftPosition ) { m_CursorTopLeftPosition = CursorTopLeftPosition; }
    inline void SetCursorPositionScale( FloatTy Scale ) { m_CursorPositionScale = Scale; }

    /*
     *
     * Register input
     *
     * */
    void      RegisterKeyUpdate( bool pressed, KeyIDTY id );
    void      PushCharInput( uint32_t CodePoint );
    KeyState& GetKeyState( KeyIDTY id );

    void Update( );

    [[nodiscard]] KeyState& GetPrimaryKey( );
    [[nodiscard]] KeyState& GetSecondaryKey( );
    [[nodiscard]] KeyState& GetFunctionKey( );

    /*
     *
     * Alter input mode
     *
     * */
    void EnableRowCursorMotion( bool Enable );
    void LockCursor( bool Lock );
    bool IsCursorLocked( ) const noexcept { return m_IsCursorLocked; }

    /*
     *
     * Cursor
     *
     * */
    [[nodiscard]] inline FloatTy             GetCursorSensitivity( ) const { return m_CursorSensitivity; }
    [[nodiscard]] inline std::pair<int, int> GetCursorDeltaPosition( ) const { return m_CursorDeltaPosition; }
    [[nodiscard]] inline std::pair<int, int> GetCursorPosition( ) const { return ( m_CursorPosition - m_CursorTopLeftPosition ) * m_CursorPositionScale; }
    [[nodiscard]] inline std::pair<int, int> GetCursorTopLeftPosition( ) const { return m_CursorTopLeftPosition; }
    [[nodiscard]] inline FloatTy             GetCursorPositionScale( ) const { return m_CursorPositionScale; }

    /*
     *
     * Stats
     *
     * */
    [[nodiscard]] inline uint32_t GetFrameCount( ) const { return m_FrameCount; }
    [[nodiscard]] inline int32_t  GetKeyPressCount( ) const { return m_KeyPressingCount; }
    [[nodiscard]] inline auto&    GetNewKeyPresses( ) const { return m_NewKeyPresses.first; }
    [[nodiscard]] inline auto&    GetNewCharPresses( ) const { return m_NewCharPresses.first; }

    /*
     *
     * GLFW context
     *
     * */
    void
    StoreGLFWContext( );

private:
    uint32_t    m_FrameCount = 0;
    GLFWwindow* m_EventWindow { };

    KeyState m_PrimaryKey;
    KeyState m_SecondaryKey;
    KeyState m_FunctionKey;

    // When logical size and different from physical size
    // Multiply to get logical location from physical location
    // Set by engine
    FloatTy             m_CursorPositionScale { 1 };
    std::pair<int, int> m_CursorTopLeftPosition;
    std::pair<int, int> m_CursorDeltaPosition { };
    std::pair<int, int> m_CursorPosition;
    bool                m_IsCursorLocked = false;

    // TODO: Use list
    std::map<KeyIDTY, KeyState> m_OtherKeysStates { };

    bool    m_EnableRowCursorMotion { false };
    FloatTy m_CursorSensitivity { 0.5f };

    // All key presses this frame
    std::pair<std::vector<uint32_t>, std::vector<uint32_t>> m_NewCharPresses;
    std::pair<std::vector<KeyIDTY>, std::vector<KeyIDTY>>   m_NewKeyPresses;
    int32_t                                                 m_KeyPressingCount { };

    void UpdateMouse( );

    /*
     *
     * GLFW context
     *
     * */

    void ( *glfwSetInputModePtr )( ) = nullptr;
};