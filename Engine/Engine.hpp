//
// Created by loyus on 7/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <string>

class Engine
{
    /*
     *
     * Engine's render job
     *
     * */
    void
    Render( );

public:
    Engine( );
    ~Engine( );

    /*
     *
     * Engine's main update loop
     * This will also invoke UpdateRootConcept
     *
     * */
    void
    Update( );

    /*
     *
     * Engine's main update loop
     *
     * */
    void
    UpdateRootConcept( );

    /*
     *
     * Return whether the engine should be in a update loop
     *
     * */
    bool
    ShouldShutdown( );

    /*
     *
     * Load project, this has to be called from engine
     * because it will initialize the concept
     *
     * */
    void
    LoadProject( const std::string& Path );

    /*
     *
     * current opened project
     *
     * */
    class Project*
    GetProject( ) const;

    struct GladGLContext*
    GetGLContext( );

    static Engine*
    GetEngine( );

    static void
    OnKeyboardInput( struct GLFWwindow* window, int key, int scancode, int action, int mods );

    static void
    OnWindowResize( struct GLFWwindow* window, int Width, int Height );

    /*
     *
     * Set the global engine context
     * This can be useful when dynamic libraries are used
     *
     * */
    static void
    SetEngine( Engine* EngineContext );

    /*
     *
     * Time related
     *
     * */
    void
    ResetTimer( );

    FloatTy
    GetDeltaSecond( ) const;

    TimerTy::time_point
    GetCurrentTime( ) const;

    /*
     *
     * Used fot how reloaded library to load context correctly
     *
     * */
    void
    MakeMainWindowCurrentContext( );

    /*
     *
     * Main window related
     *
     * */
    std::pair<int, int>
    GetMainWindowViewPortDimensions( ) const;

    void
    SetMainWindowViewPortDimensions( std::pair<int, int> Dimension );

    /*
     *
     * Audio related
     *
     * */
    class AudioEngine*
    GetAudioEngine( );

    /*
     *
     * Input related
     *
     * */
    class UserInput*
    GetUserInputHandle( );

    void ( *GetConceptToImGuiFuncPtr( uint64_t ConceptTypeID ) )( const char*, void* );

private:
    bool m_ShouldShutdown = false;

    /*
     *
     * Input
     *
     * */
    class UserInput* m_UserInput;

    /*
     *
     * Time related
     *
     * */
    TimerTy::time_point m_FirstUpdateTime;
    TimerTy::time_point m_LastUpdateTime;
    TimerTy::time_point m_CurrentUpdateTime;
    // This will be set at the start of the update loop
    FloatTy m_DeltaSecond = 0.0f;

    /*
     *
     * Main view port size
     * ImGui window size on editor build
     * Game window size in stand-alone build
     *
     * */
    std::pair<float, float> m_MainViewPortDimensions { };

    /*
     *
     * Windows
     *
     * */
    class PrimaryWindow* m_MainWindow     = nullptr;
    class WindowPool*    m_MainWindowPool = nullptr;

    /*
     *
     * Glad / GL
     *
     * */
    struct GladGLContext* m_GLContext = nullptr;

    /*
     *
     * ImGui
     *
     * */
    struct ImGuiContext* m_ImGuiContext = nullptr;

    /*
     *
     * Should only be call after m_MainWindow has been initialized
     *
     * */
    void
    CreateImGuiContext( );
    void
    DestroyImGuiContext( );

    /*
     *
     * Audio
     *
     * */
    class AudioEngine* m_AudioEngine = nullptr;

    /*
     *
     * Root concept in the screen
     *
     * */
    class RootConceptTy* m_RootConcept = nullptr;

    /*
     *
     * Active instance of current opened project
     *
     * */
    class Project* m_ActiveProject = nullptr;

    /*
     *
     * Global instance of the engine
     *
     * */
    inline static Engine* g_Engine = nullptr;
};