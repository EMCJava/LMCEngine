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
        if ( RootConcept != nullptr )
        {
            RootConcept->GetConcepts( m_Concepts );

            static ImGuiTreeNodeFlags base_flags         = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            static bool               test_drag_and_drop = false;
            ImGui::Checkbox( "Test tree node as drag source", &test_drag_and_drop );
            ImGui::Text( "Hello!" );
            ImGui::Unindent( ImGui::GetTreeNodeToLabelSpacing( ) );

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = 0;
            int        node_clicked   = -1;

            m_Concepts.ForEachIndex( [ &node_clicked ]( auto Index, auto* Con ) {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.

                ImGuiTreeNodeFlags node_flags  = base_flags;
                const bool         is_selected = ( selection_mask & ( 1 << Index ) ) != 0;
                if ( is_selected )
                    node_flags |= ImGuiTreeNodeFlags_Selected;

                // Items 3..5 are Tree Leaves
                // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;   // ImGuiTreeNodeFlags_Bullet
                ImGui::TreeNodeEx( (void*) (intptr_t) Index, node_flags, "%s", Con->GetName() );
                if ( ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( ) )
                    node_clicked = Index;
            } );

            //            for ( int i = 0; i < 6; i++ )
            //            {
            //                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
            //                // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
            //                ImGuiTreeNodeFlags node_flags  = base_flags;
            //                const bool         is_selected = ( selection_mask & ( 1 << i ) ) != 0;
            //                if ( is_selected )
            //                    node_flags |= ImGuiTreeNodeFlags_Selected;
            //                if ( i < 3 )
            //                {
            //                    // Items 0..2 are Tree Node
            //                    bool node_open = ImGui::TreeNodeEx( (void*) (intptr_t) i, node_flags, "Selectable Node %d", i );
            //                    if ( ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( ) )
            //                        node_clicked = i;
            //                    if ( test_drag_and_drop && ImGui::BeginDragDropSource( ) )
            //                    {
            //                        ImGui::SetDragDropPayload( "_TREENODE", NULL, 0 );
            //                        ImGui::Text( "This is a drag and drop source" );
            //                        ImGui::EndDragDropSource( );
            //                    }
            //                    if ( node_open )
            //                    {
            //                        ImGui::BulletText( "Blah blah\nBlah Blah" );
            //                        ImGui::TreePop( );
            //                    }
            //                } else
            //                {
            //                    // Items 3..5 are Tree Leaves
            //                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
            //                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
            //                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;   // ImGuiTreeNodeFlags_Bullet
            //                    ImGui::TreeNodeEx( (void*) (intptr_t) i, node_flags, "Selectable Leaf %d", i );
            //                    if ( ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( ) )
            //                        node_clicked = i;
            //                    if ( test_drag_and_drop && ImGui::BeginDragDropSource( ) )
            //                    {
            //                        ImGui::SetDragDropPayload( "_TREENODE", NULL, 0 );
            //                        ImGui::Text( "This is a drag and drop source" );
            //                        ImGui::EndDragDropSource( );
            //                    }
            //                }
            //            }

            if ( node_clicked != -1 )
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if ( ImGui::GetIO( ).KeyCtrl )
                    selection_mask ^= ( 1 << node_clicked );   // CTRL+click to toggle
                else                                           // if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = ( 1 << node_clicked );    // Click to single-select
            }

            ImGui::Indent( ImGui::GetTreeNodeToLabelSpacing( ) );
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
