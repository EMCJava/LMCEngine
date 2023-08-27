//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Concept/Concept.hpp>

#include "Window.hpp"

#include <filesystem>
#include <memory>
#include <thread>
#include <map>

class EditorWindow : public GameWindow
{
    struct ConceptInspectionCache {
        /*
         *
         * Concept for Hierarchy showing
         *
         * */
        std::map<uint64_t, ConceptSetFetchCache<PureConcept>> ConceptTree { };

        /*
         *
         * Concept for Hierarchy showing
         *
         * */
        PureConcept* SelectedConcept { };

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

    void
    ConceptMemoryViewGroup( const char* Name, PureConcept* Con );


    enum class BuildStage { CMakeConfig,
                            CmakeBuild,
                            CopyFiles,
                            Finished,
                            None };
    void
    SetBuildPath( );
    void
    BuildReleaseConfigCmake( );
    void
    BuildRelease( );
    void
    BuildReleaseCopyEssentialFiles( );

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

    void
    SetRootConcept( class RootConceptTy* RootConcept ) override;

private:
    ConceptInspectionCache m_ConceptInspectionCache { };

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

    /*
     *
     * Editor states
     *
     * */
    class PureConcept *m_ConceptDragStart, *m_ConceptDragEnd;

    /*
     *
     * Build settings
     *
     * */
    std::filesystem::path        m_BuildPath { };
    bool                         m_ShouldBuild = true;
    std::unique_ptr<std::thread> m_BuildThread { };
    BuildStage                   m_BuildStage    = BuildStage::None;
    BuildStage                   m_BuildFailedAt = BuildStage::None;

    std::unique_ptr<struct LogGroup> m_BuildThreadStrBuffer;
    std::mutex                       m_BuildThreadStrBufferMutex { };

    uint32_t m_MaxThreadAllowed   = 0;
    uint32_t m_BuildThreadAllowed = 0;
};
