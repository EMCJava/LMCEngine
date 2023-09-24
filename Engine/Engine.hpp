//
// Created by loyus on 7/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <string>
#include <memory>

namespace spdlog
{
class logger;
}

template <typename>
struct ImVector;

template <class>
class ConceptSetFetchCache;

class Engine
{
    /*
     *
     * Engine's render job
     *
     * */
    void
    Render( );

    void
    SetupGlobalResources( );

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
    OnMouseButtonInput( struct GLFWwindow* window, int button, int action, int mods );

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
    GetPhysicalMainWindowViewPortDimensions( ) const;
    void
    SetPhysicalMainWindowViewPortDimensions( std::pair<int, int> Dimension );

    std::pair<int, int>
    GetLogicalMainWindowViewPortDimensions( ) const;

    std::pair<int, int>
    GetMainWindowViewPortDimensions( ) const;

    /*
     *
     * Logger related
     *
     * */
    struct LogGroup*
    GetEngineDefaultLogGroup( );

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

    /*
     *
     * Make sure all code are using the same instance
     *
     * */
    class GlobalResourcePool*
    GetGlobalResourcePool( );

    void ( *GetConceptToImGuiFuncPtr( uint64_t ConceptTypeID ) )( const char*, void* );

    ImVector<struct ImRect>* GetImGuiGroupPanelLabelStack( ) { return m_ImGuiGroupPanelLabelStack; }

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
     * If m_ViewportLogicalDimensionSetting is 0 (default), game viewport size will be actual pixel size (m_ViewportPhysicalDimension)
     * else game viewport size will rescale within m_ViewportPhysicalDimension with same aspect ratio
     *
     * */
    std::pair<int, int> m_ViewportLogicalDimensionSetting { 1920, 1080 };
    std::pair<int, int> m_ViewportLogicalDimension { };   // Avoid checking every frame

    std::pair<int, int> m_ViewportPhysicalDimension { };
    std::pair<int, int> m_ViewportDimension { };

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
    struct ImGuiContext*     m_ImGuiContext              = nullptr;
    ImVector<struct ImRect>* m_ImGuiGroupPanelLabelStack = nullptr;

    void* ( *m_ImGuiContext_alloc_func )( size_t sz, void* user_data ) = nullptr;
    void ( *m_ImGuiContext_free_func )( void* ptr, void* user_data )   = nullptr;
    void* m_ImGuiContext_user_data                                     = nullptr;

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
     * Logging
     *
     * */
    std::shared_ptr<spdlog::logger> m_DefaultLogger;
    struct LogGroup*                m_DefaultLogGroup = nullptr;

    /*
     *
     *
     *
     * */
    class GlobalResourcePool* m_GlobalResourcePool = nullptr;

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
    class RootConceptTy*                           m_RootConcept         = nullptr;
    ConceptSetFetchCache<class ConceptApplicable>* m_RootApplicableCache = nullptr;

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