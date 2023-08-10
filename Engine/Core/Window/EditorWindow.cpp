//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <imgui.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/File/OSFile.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Engine.hpp>

#include <filesystem>

void
EditorWindow::Update( )
{
    m_HRFrameBuffer->BindFrameBuffer( );
    const auto WindowDimension = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );
    if ( m_MainViewPortDimension != WindowDimension )
    {
        // Size changed, need to recreate the frame buffer
        m_HRFrameBuffer->RescaleFrameBuffer( WindowDimension.first, WindowDimension.second );
    }

    // Render main game
    GameWindow::Update( );
    m_HRFrameBuffer->UnBindFrameBuffer( );

    m_PreviousFrameTextureID = m_HRFrameBuffer->GetTextureID( );
}

void
EditorWindow::SetPreviousFrameTexture( uint32_t TextureID )
{
    m_PreviousFrameTextureID = TextureID;
}

EditorWindow::EditorWindow( int Width, int Height, const char* Title, bool Fullscreen, bool Create )
    : GameWindow( Width, Height, Title, Fullscreen, Create )
{
    m_HRFrameBuffer = new HotReloadFrameBuffer;
    m_HRFrameBuffer->SetGLContext( m_GLContext );
    m_HRFrameBuffer->CreateFrameBuffer( m_Width, m_Height );
}

EditorWindow::EditorWindow( const char* Title, bool Create )
    : GameWindow( Title, Create )
{
    m_HRFrameBuffer = new HotReloadFrameBuffer;
    m_HRFrameBuffer->SetGLContext( m_GLContext );
    m_HRFrameBuffer->CreateFrameBuffer( m_Width, m_Height );
}

EditorWindow::~EditorWindow( )
{
    delete m_HRFrameBuffer;
    m_HRFrameBuffer = nullptr;
}

void
EditorWindow::UpdateImGui( )
{
    MakeContextCurrent( );
    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    gl->Viewport( 0, 0, m_Width, m_Height );
    gl->ClearColor( 0, 0, 0, 0 );
    gl->Clear( GL_COLOR_BUFFER_BIT );

    ImGui::DockSpaceOverViewport( ImGui::GetMainViewport( ) );

    if ( m_ShowImGuiDemoWindow )
    {
        ImGui::ShowDemoWindow( );
    }

    if ( ImGui::BeginMainMenuBar( ) )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem( "Open project" ) )
            {
                spdlog::info( "Open project" );
                const auto ProjectPath = OSFile::PickFile( {
                    {"LMCEngine project file", "lmce"}
                } );
                if ( ProjectPath.empty( ) )
                {
                    spdlog::info( "Operation cancelled" );
                } else
                {
                    Engine::GetEngine( )->LoadProject( ProjectPath );

                    // Load preset layout, need to do it here for the exception to be thrown
                    const auto LayoutPath = std::filesystem::path { ProjectPath }.parent_path( ) / "Editor/layout.ini";
                    if ( std::filesystem::exists( LayoutPath ) )
                    {
                        Engine::GetEngine( )->GetProject( )->GetConfig( ).editor_layout_path = LayoutPath.string( );
                        throw ImGuiContextInvalid { };
                    }
                }
            }

            if ( ImGui::MenuItem( "Save project" ) )
            {
                spdlog::info( "Save project" );
                Engine::GetEngine( )->GetProject( )->SaveProject( );
            }

            ImGui::Separator( );
            if ( ImGui::MenuItem( "Close", "Alt+F4" ) )
            {
                glfwSetWindowShouldClose( m_Window, GL_TRUE );
            }

            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "View" ) )
        {
            if ( ImGui::BeginMenu( "Layout" ) )
            {
                auto& ImGuiIO = ImGui::GetIO( );
                if ( ImGui::MenuItem( "Save Layout", nullptr, false, ImGuiIO.WantSaveIniSettings ) )
                {
                    const auto& DefaultPathStr = Engine::GetEngine( )->GetProject( )->GetConfig( ).editor_layout_path;
                    const char* DefaultPath    = nullptr;
                    if ( !DefaultPathStr.empty( ) )
                    {
                        DefaultPath = DefaultPathStr.c_str( );
                    }

                    const auto SaveLocation = OSFile::SaveFile( {
                                                                    {"Layout init file", "ini"}
                    },
                                                                DefaultPath );
                    if ( !SaveLocation.empty( ) )
                    {
                        ImGui::SaveIniSettingsToDisk( SaveLocation.c_str( ) );
                        ImGuiIO.WantSaveIniSettings = false;
                    }
                }

                if ( ImGui::MenuItem( "Load Layout" ) )
                {
                    const auto LoadLocation = OSFile::PickFile( {
                        {"Layout init file", "ini"}
                    } );
                    if ( !LoadLocation.empty( ) )
                    {
                        Engine::GetEngine( )->GetProject( )->GetConfig( ).editor_layout_path = LoadLocation;
                        throw ImGuiContextInvalid { };
                    }
                }

                ImGui::EndMenu( );
            }

            ImGui::Separator( );
            ImGui::MenuItem( "ImGui Demo Window", nullptr, &m_ShowImGuiDemoWindow );

            ImGui::EndMenu( );
        }

        if ( ImGui::BeginMenu( "Build" ) )
        {
            if ( ImGui::MenuItem( "Build Project" ) )
            {
            }

            ImGui::EndMenu( );
        }

        ImGui::EndMainMenuBar( );
    }

    {
        ImGui::Begin( "Hierarchy" );

        auto* RootConcept = GetConceptPtr( );
        if ( RenderConceptHierarchy( RootConcept ) )
        {
            m_ConceptInspectionCache.SelectedConceptMask.clear( );
            m_ConceptInspectionCache.SelectedConceptMask.emplace( RootConcept );
        }

        ImGui::End( );
    }

    {
        ImGui::Begin( "Console" );

        const auto& io = ImGui::GetIO( );
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate );
        ImGui::End( );
    }

    {
        ImGui::Begin( "Details" );

        if ( m_ConceptInspectionCache.m_SelectedConcept != nullptr )
        {
            ImGui::Text( "Concept type - %s -", m_ConceptInspectionCache.m_SelectedConcept->GetName( ) );
        }

        ImGui::End( );
    }

    {
        ImGui::Begin( "Main viewport" );

        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        ImGui::BeginChild( "Render" );

        const auto                WindowDimensions    = ImGui::GetContentRegionAvail( );
        const std::pair<int, int> WindowDimensionPair = { WindowDimensions.x, WindowDimensions.y };
        if ( m_MainViewPortDimension != WindowDimensionPair )
        {
            Engine::GetEngine( )->SetMainWindowViewPortDimensions( WindowDimensionPair );
        }

        ImGui::Image( reinterpret_cast<void*>( m_PreviousFrameTextureID ),
                      WindowDimensions, ImVec2( 0, 1 ), ImVec2( 1, 0 ) );

        ImGui::EndChild( );

        ImGui::End( );
    }
}

bool
EditorWindow::RenderConceptHierarchy( PureConcept* Con )
{
    bool IsSelected = false;

    static const auto DragAndDropOperation = []( PureConcept* Con ) {
        /*
         *
         * Drag and drop operations
         *
         * */
        if ( ImGui::BeginDragDropSource( ) )
        {
            ImGui::SetDragDropPayload( "ConceptDAD", &Con, sizeof( PureConcept* ) );
            ImGui::Text( "Moves as sub-concept" );
            ImGui::EndDragDropSource( );
        }

        if ( ImGui::BeginDragDropTarget( ) )
        {
            if ( const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload( "ConceptDAD" ) )
            {
                PureConcept* PayloadConcept = *( (PureConcept**) Payload->Data );
                spdlog::info( "Drag concept from {} to {}", PayloadConcept->GetName( ), Con->GetName( ) );
            }
            ImGui::EndDragDropTarget( );
        }
    };

    if ( Con != nullptr )
    {
        ImGuiTreeNodeFlags BaseFlag    = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        const bool         is_selected = m_ConceptInspectionCache.SelectedConceptMask.contains( Con );
        if ( is_selected )
        {
            BaseFlag |= ImGuiTreeNodeFlags_Selected;
            m_ConceptInspectionCache.m_SelectedConcept = Con;
        }

        if ( Con->CanCastV( Concept::TypeID ) && ( (Concept*) Con )->HasSubConcept( ) )
        {
            // Has sub node
            bool node_open = ImGui::TreeNodeEx( Con, BaseFlag, "%s", Con->GetName( ) );
            IsSelected     = ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( );

            DragAndDropOperation( Con );

            if ( node_open )
            {
                auto& ConceptsCache = m_ConceptInspectionCache.m_ConceptTree[ Con->GetHash( ) ];
                ( (Concept*) Con )->GetConcepts( ConceptsCache );

                ImGui::Unindent( ImGui::GetTreeNodeToLabelSpacing( ) );

                ConceptsCache.ForEachIndex( [ this ]( auto Index, PureConcept* Con ) {
                    ImGui::PushID( Con );

                    if ( RenderConceptHierarchy( Con ) )
                    {
                        m_ConceptInspectionCache.SelectedConceptMask.clear( );
                        m_ConceptInspectionCache.SelectedConceptMask.emplace( Con );
                    }

                    ImGui::PopID( );
                } );

                ImGui::Indent( ImGui::GetTreeNodeToLabelSpacing( ) );

                ImGui::TreePop( );
            }
        } else
        {
            // No sub node
            BaseFlag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx( Con, BaseFlag, "%s", Con->GetName( ) );
            IsSelected = ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( );

            DragAndDropOperation( Con );
        }
    }

    return IsSelected;
}
