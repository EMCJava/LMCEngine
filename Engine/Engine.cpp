//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <ImGui/ImGui.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <Engine/Library/ImGui/Ext/LogGroup.hpp>

#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultTextureShaderInstancing.hpp>
#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultColorPreVertexShader.hpp>
#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultTextureShader.hpp>
#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultColorShader.hpp>
#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultPhongShader.hpp>
#include <Engine/Core/Environment/GlobalConstResources/SahderCode/DefaultFontShader.hpp>
#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquare.hpp>
#include <Engine/Core/Graphic/Shader/ShaderProgram.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCameraStack.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
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
#include <Engine/Core/UI/Font/Font.hpp>

#include RootConceptIncludePath

#include <regex>
#include <string>
#include <string_view>

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
Engine::OnMouseButtonInput( struct GLFWwindow* window, int button, int action, int mods )
{
    (void) mods;
    static const auto* EnginePtr = Engine::GetEngine( );

    if ( action != GLFW_REPEAT )
    {
        EnginePtr->m_UserInput->RegisterKeyUpdate( action == GLFW_PRESS, button );
    }
}

void
Engine::OnWindowResize( struct GLFWwindow* /*unused*/, int Width, int Height )
{
    // Should only be used when in non editor mode.
    // Editor mode will override it.
    Engine::GetEngine( )->SetPhysicalMainWindowViewPortDimensions( { Width, Height } );
}

Engine::Engine( )
{
    if ( g_Engine != nullptr )
    {
        throw std::runtime_error( "There is already a Engine instance!" );
    }

    spdlog::info( "Engine initializing" );
#ifndef NDEBUG
    spdlog::set_level( spdlog::level::trace );
#endif

    InitializeEnvironment( );

    m_DefaultLogGroup = new LogGroup;

    m_AudioEngine = new AudioEngine;

    m_ActiveProject = new Project;

#ifndef LMC_EDITOR

    LoadProject( GAME_CONFIG_PATH );

#endif

    m_ImGuiGroupPanelLabelStack = new ImVector<ImRect>;

    m_MainWindow = new PrimaryWindow( 1920, 1080, m_ActiveProject->GetConfig( ).project_name.c_str( ) );
    m_GLContext  = m_MainWindow->GetGLContext( );

    m_UserInput = new UserInput( m_MainWindow->GetWindowHandle( ) );

    glfwSetKeyCallback( m_MainWindow->GetWindowHandle( ), OnKeyboardInput );
    glfwSetMouseButtonCallback( m_MainWindow->GetWindowHandle( ), OnMouseButtonInput );

    // Setup Dear ImGui context
    CreateImGuiContext( );

    m_MainWindowPool = new WindowPool;

#ifndef LMC_EDITOR
    auto ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
    auto FileSink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>( "ProjectCache/EngineLog.log" );
    m_DefaultLogger  = std::make_shared<spdlog::logger>( "EngineSink", spdlog::sinks_init_list { ConsoleSink, FileSink } );
#else

    auto ConsoleSink  = std::make_shared<spdlog::sinks::stdout_color_sink_mt>( );
    auto FileSink     = std::make_shared<spdlog::sinks::basic_file_sink_mt>( "ProjectCache/EngineLog.log" );
    auto CallBackSink = std::make_shared<spdlog::sinks::callback_sink_mt>( [ this ]( const spdlog::details::log_msg& msg ) {
        m_DefaultLogGroup->AddLog( "%s\n", std::string( msg.payload.data( ), msg.payload.size( ) ).c_str( ) );
    } );

    m_DefaultLogger = std::make_shared<spdlog::logger>( "EngineSink", spdlog::sinks_init_list { ConsoleSink, FileSink, CallBackSink } );
#endif

    /*
     *
     * Engine
     *
     * */
    SetEngine( this );

    SetupGlobalResources( );

#ifndef LMC_EDITOR

    m_RootConcept = new RootConceptTy;
    m_MainWindow->SetRootConcept( m_RootConcept );
    glfwSetWindowSizeCallback( m_MainWindow->GetWindowHandle( ), OnWindowResize );
    SetLogicalMainWindowViewPortDimensions( { 1920, 1080 } );   // default resolution
    ResetTimer( );
#endif

    m_RootApplicableCache = new ConceptSetFetchCache<ConceptApplicable>;

    spdlog::info( "========== Engine initialized ==========" );
}

Engine::~Engine( )
{
    spdlog::info( "Engine destroying" );

    // Remove concepts caches
    ResetAllDLLDependencies( );

    delete m_RootApplicableCache;
    m_RootApplicableCache = nullptr;

#ifdef LMC_EDITOR
    // We might still need the destructor for example in the dll
    // We only deallocate the concept but leave the library loaded
    if ( m_RootConcept != nullptr )
    {
        m_RootConcept->DeAllocateConcept( );
    }
#else
    delete m_RootConcept;
    m_RootConcept = nullptr;
#endif

    GetGlobalResourcePool( )->Clear( );

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

    delete m_ImGuiGroupPanelLabelStack;
    m_ImGuiGroupPanelLabelStack = nullptr;

    delete m_ActiveProject;
    m_ActiveProject = nullptr;

    delete m_AudioEngine;
    m_AudioEngine = nullptr;

#ifdef LMC_EDITOR
    // After "almost" everything is deallocated
    // We then unload the dll safely
    delete m_RootConcept;
    m_RootConcept = nullptr;
#endif

    delete m_DefaultLogGroup;
    m_DefaultLogGroup = nullptr;

    m_DefaultLogger.reset( );

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
#ifdef LMC_EDITOR
    if ( m_RootConcept != nullptr )
    {
        auto& RootConcept = *m_RootConcept;

        if ( RootConcept.ShouldReload( ) ) [[unlikely]]
        {
            spdlog::info( "RootConcept changes detected, hot reloading" );

            ResetAllDLLDependencies( );
            RootConcept.Reload( false );

            spdlog::info( "Reloading Audio engine" );

            delete m_AudioEngine;
            m_AudioEngine = new AudioEngine;

            /*
             *
             * Make sure the library can access the same memory space
             *
             * */
            RootConcept.SetEngineContext( this );
            RootConcept.AllocateConcept( );
            m_MainWindow->SetRootConcept( m_RootConcept );
            m_MainWindow->MakeContextCurrent( );

            ResetTimer( );

            /*
             *
             * Make sure new root concept reset camera
             *
             * */
            m_ViewportPhysicalDimension = { };
        }

        TEST( RootConcept->CanCastVT<ConceptApplicable>( ) )
        RootConcept.As<ConceptApplicable>( )->Apply( );

        RootConcept.As<Concept>( )->GetConcepts( *m_RootApplicableCache );
        m_RootApplicableCache->ForEach( []( std::shared_ptr<ConceptApplicable>& Applicable ) {
            Applicable->Apply( );
        } );
    }
#else
    m_RootConcept->Apply( );
    m_RootConcept->GetConcepts( *m_RootApplicableCache );
    m_RootApplicableCache->ForEach( []( std::shared_ptr<ConceptApplicable>& Applicable ) {
        Applicable->Apply( );
    } );
#endif
}

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
        spdlog::info( "Destroying ImGui context: {}", fmt::ptr( m_ImGuiContext ) );
        DestroyImGuiContext( );
    }

    ImGui::GetAllocatorFunctions( &m_ImGuiContext_alloc_func, &m_ImGuiContext_free_func, &m_ImGuiContext_user_data );
    m_ImGuiContext = ImGui::CreateContext( );
    spdlog::info( "New ImGui context: {}", fmt::ptr( m_ImGuiContext ) );

    ImGui::SetCurrentContext( m_ImGuiContext );
    ImGuiIO& io = ImGui::GetIO( );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;
    if ( !m_MainWindow->IsFullscreen( ) )
    {
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
    ImGui::StyleColorsLight( );
    ImGui::GetStyle( ).SeparatorTextAlign = ImVec2( 0.5F, 0.5F );

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL( m_MainWindow->GetWindowHandle( ), true );
    ImGui_ImplOpenGL3_Init( );

#ifdef LMC_EDITOR
    if ( m_RootConcept != nullptr )
    {
        m_RootConcept->SetEngineContext( this );
    }
#endif
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

#ifdef LMC_EDITOR
    if ( m_RootConcept != nullptr )
    {
        ResetAllDLLDependencies( );

        delete m_RootConcept;
        m_RootConcept = nullptr;
    }

    const auto& RootConcept = m_ActiveProject->GetConfig( ).root_concept;

    if ( !RootConcept.empty( ) )
    {
        spdlog::info( "Reloading Audio engine" );

        delete m_AudioEngine;
        m_AudioEngine = new AudioEngine;

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
    REQUIRED_IF( EngineContext->m_DefaultLogger != nullptr )
    {
        spdlog::set_default_logger( EngineContext->m_DefaultLogger );
    }

#ifndef NDEBUG
    spdlog::set_level( spdlog::level::trace );
#endif

    spdlog::warn( "Setting engine context manually to: {}", fmt::ptr( EngineContext ) );
    g_Engine = EngineContext;

    spdlog::warn( "Setting ImGui context manually to: {}", fmt::ptr( EngineContext->m_ImGuiContext ) );
    ImGui::SetCurrentContext( EngineContext->m_ImGuiContext );
    ImGui::SetAllocatorFunctions( EngineContext->m_ImGuiContext_alloc_func, EngineContext->m_ImGuiContext_free_func, EngineContext->m_ImGuiContext_user_data );
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
Engine::GetPhysicalMainWindowViewPortDimensions( ) const
{
    return m_ViewportPhysicalDimension;
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
Engine::SetPhysicalMainWindowViewPortDimensions( std::pair<int, int> Dimension )
{
    m_ViewportPhysicalDimension = Dimension;

    if ( m_ViewportLogicalDimensionSetting.first == 0 || m_ViewportLogicalDimensionSetting.second == 0 )
    {
        m_ViewportDimension = m_ViewportLogicalDimension = m_ViewportPhysicalDimension;
        m_UserInput->SetCursorPositionScale( 1 );
    } else
    {
        const auto WidthFollowHeight = ( static_cast<FloatTy>( m_ViewportLogicalDimensionSetting.first ) / m_ViewportLogicalDimensionSetting.second ) * m_ViewportPhysicalDimension.second;
        if ( WidthFollowHeight <= m_ViewportPhysicalDimension.first )
        {
            m_ViewportDimension = { WidthFollowHeight, m_ViewportPhysicalDimension.second };
            m_UserInput->SetCursorPositionScale( static_cast<FloatTy>( m_ViewportLogicalDimensionSetting.second ) / m_ViewportPhysicalDimension.second );
        } else
        {
            m_ViewportDimension = { m_ViewportPhysicalDimension.first, ( static_cast<FloatTy>( m_ViewportLogicalDimensionSetting.second ) / m_ViewportLogicalDimensionSetting.first ) * m_ViewportPhysicalDimension.first };
            m_UserInput->SetCursorPositionScale( static_cast<FloatTy>( m_ViewportLogicalDimensionSetting.first ) / m_ViewportPhysicalDimension.first );
        }
        m_ViewportLogicalDimension = m_ViewportLogicalDimensionSetting;
    }
}

LogGroup*
Engine::GetEngineDefaultLogGroup( )
{
    return m_DefaultLogGroup;
}

void
Engine::SetupGlobalResources( )
{
    auto SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultColorShaderVertexSource, DefaultColorShaderFragmentSource );
    auto SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultColorShader", std::move( SpriteShader ) );

    SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultColorPreVertexShaderVertexSource, DefaultColorPreVertexShaderFragmentSource );
    SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultColorPreVertexShader", std::move( SpriteShader ) );

    SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultTextureShaderVertexSource, DefaultTextureShaderFragmentSource );
    SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultTextureShader", std::move( SpriteShader ) );

    SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultFontShaderVertexSource, DefaultFontShaderFragmentSource );
    SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultFontShader", std::move( SpriteShader ) );

    SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultPhongShaderVertexSource, DefaultPhongShaderFragmentSource );
    SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultPhongShader", std::move( SpriteShader ) );

    SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( DefaultTextureShaderInstancingVertexSource, DefaultTextureShaderInstancingFragmentSource );
    SpriteShader = std::make_shared<Shader>( );
    SpriteShader->SetProgram( SProgram );
    GlobalResourcePool::STryPush( "DefaultTextureShaderInstancing", std::move( SpriteShader ) );

    /*
     *
     * Font loading
     *
     * */
    auto DefaultFont = std::make_shared<Font>( );
    DefaultFont->LoadFont( "Assets/Font/FiraCode.ttf" );
    GlobalResourcePool::GetInstance( ).STryPush( "DefaultFont", std::move( DefaultFont ) );

    /*
     *
     * Camera stack setup
     *
     * */
    GlobalResourcePool::SSet( "DefaultCameraStack", std::make_shared<PureConceptCameraStack>( ) );

    m_GlobalResourcePool = &GlobalResourcePool::GetInstance( );
}

GlobalResourcePool*
Engine::GetGlobalResourcePool( )
{
    return m_GlobalResourcePool;
}
std::pair<int, int>
Engine::GetLogicalMainWindowViewPortDimensions( ) const
{
    return m_ViewportLogicalDimension;
}
std::pair<int, int>
Engine::GetMainWindowViewPortDimensions( ) const
{
    return m_ViewportDimension;
}

void
Engine::SetLogicalMainWindowViewPortDimensions( std::pair<int, int> Dimension )
{
    m_ViewportLogicalDimensionSetting = Dimension;

    // Attempt to reset viewport
    SetPhysicalMainWindowViewPortDimensions( m_MainWindow->GetDimensions( ) );
}

void
Engine::ResetAllDLLDependencies( )
{
    // Make sure all destructor are called before unloading library
    if ( m_RootApplicableCache != nullptr ) m_RootApplicableCache->Clear( );
    if ( m_MainWindow != nullptr ) m_MainWindow->SetRootConcept( nullptr );
    if ( m_RootApplicableCache != nullptr ) m_RootApplicableCache->Clear( );

#ifdef LMC_EDITOR
    // Should only be called in editor anyway
    if ( m_RootConcept != nullptr ) m_RootConcept->DeAllocateConcept( );
#endif

    GetGlobalResourcePool( )->Get<PureConceptCameraStack>( "DefaultCameraStack" )->Clear( );
}

void ( *Engine::GetConceptToImGuiFuncPtr( uint64_t ConceptTypeID ) )( const char*, void* )
{
#ifdef LMC_EDITOR

    REQUIRED_IF( m_RootConcept != nullptr )
    {
        const auto& Map = m_RootConcept->GetConceptToImGuiFuncPtrMap( );
        if ( const auto It = Map.find( ConceptTypeID ); It != Map.end( ) )
        {
            return It->second;
        }
    }

    return nullptr;
#else
    return nullptr;
#endif
}