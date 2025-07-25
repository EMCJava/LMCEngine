//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Concept/ConceptList.hpp>

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
        std::weak_ptr<PureConcept> SelectedConcept { };

        /*
         *
         * A mask for ImGui selection
         *
         * */
        std::set<PureConcept*> SelectedConceptMask { };
    };

    /*
     *
     * Return true if node is selected, should only be called in EditorWindow.cpp where the definition lays
     *
     * */
    bool
    RenderConceptHierarchy( std::shared_ptr<PureConcept>& ConShared );

    void
    RenderImGuizmo( const auto& RenderRect );

    /*
     *
     * Render intractable ImGuizmo gizmo
     *
     * */
    bool
    RenderImGuizmoGizmo( float* matrix );

    void
    RenderImGuizmoPanel( float* matrix );

    void
    ConceptMemoryViewGroup( const char* Name, PureConcept* Con );

    void
    RenderBuildOverlay( );

    void
    RenderMenuBar( );


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
     * ImGuizmo settings
     *
     * */
    ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE      m_CurrentGizmoMode      = ImGuizmo::LOCAL;
    bool                m_GizmoUseSnap          = false;
    FloatTy             m_GizmoSnapValue        = 1.0;
    // For Internal usage only, not valid most of the time
    const FloatTy* m_GizmoCameraProjection = nullptr;
    const FloatTy* m_GizmoCameraView       = nullptr;

    /*
     *
     * Main viewport setup
     *
     * */
    uint32_t            m_PreviousFrameTextureID { };
    std::pair<int, int> m_FrameBufferDimension { };

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
    bool                         m_ShouldActivateOverlay { };
    std::string                  m_PreferredCompiler { };
    std::string                  m_PreferredGenerator { };
    std::string                  m_VcpkgToolChain { };
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
