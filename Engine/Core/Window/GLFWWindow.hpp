//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <string>

class Window
{
public:
    /*
     *
     * Create a normal window.
     *
     * */
    Window( int Width, int Height, const char* Title, bool Fullscreen = false, bool Create = true );

    /*
     *
     * Create a fullscreen window, with full resolution.
     *
     * */
    Window( const char* Title, bool Create = true );

    ~Window( );

    /*
     *
     * Take all the config and create the window.
     *
     * */
    void
    CreateWindow( );

    /*
     *
     * The main loop of the window, polling events etc.
     *
     * */
    virtual void
    Update( ) = 0;

    /*
     *
     * The main loop of the window for rendering ImGui
     *
     * */
    virtual void
    UpdateImGui( ) { };

    /*
     *
     * Set swap interval for this window
     * Thus this function makes call to MakeContextCurrent
     *
     * */
    void
    SetSwapInterval( int interval );

    /*
     *
     * Make all calls to glfw focus this window.
     *
     * */
    void
    MakeContextCurrent( );

    /*
     *
     * Return whether user requested to close the window.
     *
     * */
    bool
    WindowShouldClose( ) const;

    /*
     *
     * Getter
     *
     * */
    struct GLFWwindow*
    GetWindowHandle( ) const;

    bool
    IsFullscreen( ) const;

    /*
     *
     * Get windows dimensions(width & height).
     *
     * */
    std::pair<int, int>
    GetDimensions( ) const;

    struct GladGLContext*
    GetGLContext( ) const;

protected:
    int         m_Width, m_Height;
    bool        m_Fullscreen;
    std::string m_Title;

    struct GLFWwindow*    m_Window       = nullptr;
    struct GladGLContext* m_GLContext    = nullptr;
    struct ImGuiContext*  m_ImGuiContext = nullptr;
};
