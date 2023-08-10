//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Concept/Concept.hpp>

#include "Window.hpp"

#include <map>

class EditorWindow : public GameWindow
{
    struct ConceptInspectionCache {
        /*
         *
         * Concept for Hierarchy showing
         *
         * */
        std::map<uint64_t, ConceptSetFetchCache<PureConcept>> m_ConceptTree { };

        /*
         *
         * Concept for Hierarchy showing
         *
         * */
        PureConcept* m_SelectedConcept { };

        /*
         *
         * A mask for ImGui selection
         *
         * */
        std::set<PureConcept*> SelectedConceptMask { };
    };


    /*
     *
     * Return true if node is selected
     *
     * */
    bool
    RenderConceptHierarchy( PureConcept* Con );

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
    ConceptInspectionCache m_ConceptInspectionCache;

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
