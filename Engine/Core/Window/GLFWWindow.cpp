//
// Created by loyus on 7/2/2023.
//

#include "GLFWWindow.hpp"

#include <Engine/Core/Graphic/API/OpenGL.hpp>
#include <Engine/Core/Core.hpp>

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <stdexcept>

static void
glfw_error_callback( int error, const char* description )
{
    spdlog::error( "Error: {}", description );
}

void
InitializeWindowEnvironment( )
{
    if ( GLFW_FALSE == glfwInit( ) )
    {
        throw std::runtime_error( "Failed to initialize GLFW" );
    }

    glfwSetErrorCallback( glfw_error_callback );
}

void
ShutdownWindowEnvironment( )
{
    glfwTerminate( );
}

Window::Window( int Width, int Height, const char* Title, bool Fullscreen, bool Create )
{
    const auto* VideoMode = glfwGetVideoMode( glfwGetPrimaryMonitor( ) );

    m_Width          = Width;
    m_Height         = Height;
    m_Title          = Title;
    m_Fullscreen     = Fullscreen;
    m_MaxRefreshRate = VideoMode->refreshRate;

    if ( Create )
    {
        CreateWindow( );
    }
}

Window::Window( const char* Title, bool Create )
{
    const auto* VideoMode = glfwGetVideoMode( glfwGetPrimaryMonitor( ) );

    m_Width          = VideoMode->width;
    m_Height         = VideoMode->height;
    m_Title          = Title;
    m_Fullscreen     = true;
    m_MaxRefreshRate = VideoMode->refreshRate;

    if ( Create )
    {
        CreateWindow( );
    }
}

Window::~Window( )
{
    if ( m_GLContext != nullptr )
    {
        gladLoaderUnloadGLContext( m_GLContext );

        delete m_GLContext;
        m_GLContext = nullptr;
    }

    if ( m_Window != nullptr )
    {
        glfwDestroyWindow( m_Window );
        m_Window = nullptr;
    }
}

void
Window::CreateWindow( )
{
#ifdef LMC_APPLE
    /* We need to explicitly ask for a 3.3 context on OS X */
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    if ( m_Fullscreen )
    {
        m_Window = glfwCreateWindow( m_Width, m_Height, m_Title.c_str( ), glfwGetPrimaryMonitor( ), nullptr );
    } else
    {
        m_Window = glfwCreateWindow( m_Width, m_Height, m_Title.c_str( ), nullptr, nullptr );
    }

    if ( m_Window == nullptr )
    {
        throw std::runtime_error( "Failed to create GLFW window" );
    }

    MakeContextCurrent( );

    /*
     *
     * Initialize GLAD & ImGui
     *
     * */
    if ( m_GLContext != nullptr )
    {
        gladLoaderUnloadGLContext( m_GLContext );

        delete m_GLContext;
        m_GLContext = nullptr;
    }

    m_GLContext                  = new GladGLContext;
    [[maybe_unused]] int version = gladLoadGLContext( m_GLContext, glfwGetProcAddress );
}

void
Window::SetSwapInterval( int interval )
{
    MakeContextCurrent( );
    glfwSwapInterval( interval );
}

void
Window::MakeContextCurrent( )
{
    glfwMakeContextCurrent( m_Window );
}

bool
Window::WindowShouldClose( ) const
{
    return glfwWindowShouldClose( m_Window );
}

struct GLFWwindow*
Window::GetWindowHandle( ) const
{
    return m_Window;
}

bool
Window::IsFullscreen( ) const
{
    return m_Fullscreen;
}

std::pair<int, int>
Window::GetDimensions( ) const
{
    return { m_Width, m_Height };
}

struct GladGLContext*
Window::GetGLContext( ) const
{
    return m_GLContext;
}

void
Window::SetTitle( std::string Title )
{
    m_Title = std::move( Title );
    if ( m_Window )
    {
        glfwSetWindowTitle( m_Window, m_Title.c_str( ) );
    }
}

const std::string&
Window::GetTitle( )
{
    return m_Title;
}
