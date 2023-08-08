//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquare.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/Environment/Environment.hpp>
#include <Engine/Core/Window/EditorWindow.hpp>
#include <Engine/Core/Window/WindowPool.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Concept/ConceptSetFetchCache.hpp>
#include <Engine/Core/Concept/ConceptApplicable.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Audio/AudioEngine.hpp>
#include <Engine/Core/Input/UserInput.hpp>

#include RootConceptIncludePath

#include <regex>

// For some reason, the following initialization will create multiple static variable for different translation unit on MAC
// So now we are using inline in header file instead
// Engine *Engine::g_Engine = nullptr;

void
Engine::OnKeyboardInput( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    (void) ( scancode, mods );
    static const auto* EnginePtr = Engine::GetEngine( );

    switch ( action )
    {
    case GLFW_PRESS:
        if ( key == GLFW_KEY_F11 )
        {
            // RecreateWindow
        }
        break;
    case GLFW_RELEASE:
    case GLFW_REPEAT:
    default:

        break;
    }

    if ( action != GLFW_REPEAT )
    {
        EnginePtr->m_UserInput->RegisterKeyUpdate( action == GLFW_PRESS, key );
    }
}

void
Engine::OnWindowResize( struct GLFWwindow* /*unused*/, int Width, int Height )
{
    Engine::GetEngine( )->SetMainWindowViewPortDimensions( { Width, Height } );
}

Engine::Engine( )
{
    if ( g_Engine != nullptr )
    {
        throw std::runtime_error( "There is already a Engine instance!" );
    }

    spdlog::info( "Engine initializing" );
    InitializeEnvironment( );

    m_AudioEngine = new AudioEngine;

    m_ActiveProject = new Project;

#ifndef HOT_RELOAD

    LoadProject( GAME_CONFIG_PATH );

#endif

    m_MainWindow = new PrimaryWindow( 1600, 800, m_ActiveProject->GetConfig( ).project_name.c_str( ) );
    m_GLContext  = m_MainWindow->GetGLContext( );

    m_UserInput = new UserInput( m_MainWindow->GetWindowHandle( ) );

    glfwSetKeyCallback( m_MainWindow->GetWindowHandle( ), OnKeyboardInput );

    // Setup Dear ImGui context
    CreateImGuiContext( );

    m_MainWindowPool = new WindowPool;

    /*
     *
     * Engine
     *
     * */
    g_Engine = this;

#ifndef HOT_RELOAD

    m_RootConcept = new RootConceptTy;
    m_MainWindow->SetRootConcept( m_RootConcept );
    glfwSetWindowSizeCallback( m_MainWindow->GetWindowHandle( ), OnWindowResize );
    SetMainWindowViewPortDimensions( m_MainWindow->GetDimensions( ) );

#endif
}

Engine::~Engine( )
{
    spdlog::info( "Engine destroying" );

    delete m_RootConcept;
    m_RootConcept = nullptr;

    delete m_MainWindowPool;
    m_MainWindowPool = nullptr;

    if ( m_ImGuiContext != nullptr )
    {
        ImGui_ImplOpenGL3_Shutdown( );
        ImGui_ImplGlfw_Shutdown( );
        ImGui::DestroyContext( m_ImGuiContext );
        m_ImGuiContext = nullptr;
    }

    delete m_UserInput;
    m_UserInput = nullptr;

    delete m_MainWindow;
    m_MainWindow = nullptr;

    delete m_ActiveProject;
    m_ActiveProject = nullptr;

    delete m_AudioEngine;
    m_AudioEngine = nullptr;

    ShutdownEnvironment( );
}

void
Engine::Update( )
{
    m_CurrentUpdateTime = TimerTy::now( );

    m_MainWindow->MakeContextCurrent( );
    glfwPollEvents( );

    {
        // Setup m_DeltaSecond
        typedef std::chrono::duration<FloatTy> fsec;
        const fsec                             fs = m_CurrentUpdateTime - m_LastUpdateTime;
        m_DeltaSecond                             = fs.count( );
    }

    m_UserInput->Update( );
    UpdateRootConcept( );

    Render( );

    m_ShouldShutdown |= m_MainWindow->WindowShouldClose( );
    m_LastUpdateTime = m_CurrentUpdateTime;
}

void
Engine::UpdateRootConcept( )
{
#ifdef HOT_RELOAD
    if ( m_RootConcept != nullptr )
    {
        auto& RootConcept = *m_RootConcept;

        if ( RootConcept.ShouldReload( ) )
        {
            spdlog::info( "RootConcept changes detected, hot reloading" );
            RootConcept.Reload( false );

            /*
             *
             * Make sure the library can access the same memory space
             *
             * */
            RootConcept.SetEngineContext( this );
            RootConcept.AllocateConcept( );
            ResetTimer( );

            /*
             *
             * Make sure new root concept reset camera
             *
             * */
            m_MainViewPortDimensions = { };
        }

        RootConcept.As<ConceptApplicable>( )->Apply( );
    }
#else
    m_RootConcept->Apply( );
#endif
}

// One instance of engine, so it's probably ok
ConceptSetFetchCache<ConceptRenderable> g_ConceptRenderables { };
ConceptSetFetchCache<PureConceptCamera> g_ConceptCameras { };

void
Engine::Render( )
{

    ImGui_ImplOpenGL3_NewFrame( );
    ImGui_ImplGlfw_NewFrame( );
    ImGui::NewFrame( );

    try
    {
        m_MainWindow->UpdateImGui( );
    }
    catch ( const ImGuiContextInvalid& e )
    {
        CreateImGuiContext( );
        return;
    }

    m_MainWindow->MakeContextCurrent( );

    ImGui::Render( );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );

    // Update and Render additional Platform Windows
    if ( ImGui::GetIO( ).ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
    {
        ImGui::UpdatePlatformWindows( );
        ImGui::RenderPlatformWindowsDefault( );
    }

    m_MainWindow->Update( );
    m_MainWindowPool->Update( );

    glfwSwapBuffers( m_MainWindow->GetWindowHandle( ) );
}

bool
Engine::ShouldShutdown( )
{
    return m_ShouldShutdown;
}

void
Engine::CreateImGuiContext( )
{
    IMGUI_CHECKVERSION( );
    if ( m_ImGuiContext != nullptr )
    {
        DestroyImGuiContext( );
    }
    m_ImGuiContext = ImGui::CreateContext( );
    ImGui::SetCurrentContext( m_ImGuiContext );
    ImGuiIO& io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;
    if ( !m_MainWindow->IsFullscreen( ) )
    {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }
    io.IniFilename = nullptr;

    std::string ConfigLayout;
    if ( m_ActiveProject != nullptr )
    {
        ConfigLayout = m_ActiveProject->GetConfig( ).editor_layout_path;
        if ( !ConfigLayout.empty( ) )
        {
            spdlog::info( "Loading layout: {}", ConfigLayout );
        }
    }

    if ( !ConfigLayout.empty( ) )
    {
        ImGui::LoadIniSettingsFromDisk( ConfigLayout.c_str( ) );
    } else
    {
        ImGui::LoadIniSettingsFromMemory( DefaultEditorImGuiLayout, strlen( DefaultEditorImGuiLayout ) );
    }

    // Setup Dear ImGui style
    ImGui::StyleColorsDark( );
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL( m_MainWindow->GetWindowHandle( ), true );
    ImGui_ImplOpenGL3_Init( );
}

void
Engine::DestroyImGuiContext( )
{
    ImGui_ImplOpenGL3_Shutdown( );
    ImGui_ImplGlfw_Shutdown( );
    ImGui::DestroyContext( m_ImGuiContext );
    m_ImGuiContext = nullptr;
}

Engine*
Engine::GetEngine( )
{
    /* Putting REQUIRE here could greatly impact performance */
    TEST( g_Engine != nullptr );
    return g_Engine;
}

void
Engine::LoadProject( const std::string& Path )
{

    m_ActiveProject->LoadProject( Path );

#ifdef HOT_RELOAD
    if ( m_RootConcept != nullptr )
    {
        delete m_RootConcept;
        m_RootConcept = nullptr;
    }

    const auto& RootConcept = m_ActiveProject->GetConfig( ).root_concept;

    if ( !RootConcept.empty( ) )
    {
        spdlog::info( "Loading root concepts: {}", RootConcept );

        m_RootConcept      = new RootConceptTy;
        const auto DLLPath = std::regex_replace( m_ActiveProject->GetConfig( ).shared_library_path_format, std::regex( "\\{\\}" ), RootConcept );

        m_RootConcept->Load( DLLPath, false );

        /*
         *
         * Make sure the library can access the same memory space
         *
         * */
        m_RootConcept->SetEngineContext( this );
        m_RootConcept->AllocateConcept( );

        m_MainWindow->SetRootConcept( m_RootConcept );

        ResetTimer( );
    } else
    {
        spdlog::info( "No root concepts specified" );
    }
#endif
}

Project*
Engine::GetProject( ) const
{
    return m_ActiveProject;
}

struct GladGLContext*
Engine::GetGLContext( )
{
    return m_GLContext;
}

void
Engine::SetEngine( Engine* EngineContext )
{
    spdlog::warn( "Setting engine context manually" );
    g_Engine = EngineContext;
}

void
Engine::ResetTimer( )
{
    m_FirstUpdateTime = m_LastUpdateTime = TimerTy::now( );
}

FloatTy
Engine::GetDeltaSecond( ) const
{
    return m_DeltaSecond;
}

TimerTy::time_point
Engine::GetCurrentTime( ) const
{
    return m_CurrentUpdateTime;
}

void
Engine::MakeMainWindowCurrentContext( )
{
    TEST( m_MainWindow != nullptr )
    m_MainWindow->MakeContextCurrent( );
}

std::pair<int, int>
Engine::GetMainWindowViewPortDimensions( ) const
{
    return m_MainViewPortDimensions;
}

AudioEngine*
Engine::GetAudioEngine( )
{
    return m_AudioEngine;
}

UserInput*
Engine::GetUserInputHandle( )
{
    return m_UserInput;
}

void
Engine::SetMainWindowViewPortDimensions( std::pair<int, int> Dimension )
{
    m_MainViewPortDimensions = Dimension;
}
