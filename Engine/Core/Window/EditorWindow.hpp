//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Concept/Concept.hpp>

#include "Window.hpp"

class EditorWindow : public GameWindow
{
public:
    /*
     *
     * Create a normal window.
     *
     * */
    EditorWindow( int Width, int Height, const char* Title, bool Fullscreen = false, bool Create = true );

    /*
     *
     * Create a fullscreen window, with full resolution.
     *
     * */
    EditorWindow( const char* Title, bool Create = true );

    ~EditorWindow( );

    /*
     *
     * Render the editor layout
     *
     * */
    void
    Update( ) override;

    /*
     *
     * The main loop of the window for rendering ImGui
     *
     * */
    void
    UpdateImGui( ) override;

    void
    SetPreviousFrameTexture( uint32_t TextureID );

private:
    /*
     *
     * Concept for Hierarchy showing
     *
     * */
    ConceptSetFetchCache<Concept> m_Concepts { };

    /*
     *
     * Main viewport setup
     *
     * */
    uint32_t m_PreviousFrameTextureID { };

    /*
     *
     * For root concept to render inside a ImGui window
     *
     * */
    class HotReloadFrameBuffer* m_HRFrameBuffer;

    /*
     *
     * Editor settings
     *
     * */
    bool m_ShowImGuiDemoWindow = false;
};
