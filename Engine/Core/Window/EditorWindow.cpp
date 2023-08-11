//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/File/OSFile.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Engine.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <filesystem>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Engine/Core/Graphic/Sprites/Sprite.hpp"
#include <imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

// https://github.com/ocornut/imgui/issues/1496
namespace
{
static ImVector<ImRect> s_GroupPanelLabelStack;

void
BeginGroupPanel( const char* name, const ImVec2& size )
{
    ImGui::BeginGroup( );

    auto cursorPos   = ImGui::GetCursorScreenPos( );
    auto itemSpacing = ImGui::GetStyle( ).ItemSpacing;
    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );

    auto frameHeight = ImGui::GetFrameHeight( );
    ImGui::BeginGroup( );

    ImVec2 effectiveSize = size;
    if ( size.x < 0.0f )
        effectiveSize.x = ImGui::GetContentRegionAvail( ).x;
    else
        effectiveSize.x = size.x;
    ImGui::Dummy( ImVec2( effectiveSize.x, 0.0f ) );

    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::BeginGroup( );
    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::TextUnformatted( name );
    auto labelMin = ImGui::GetItemRectMin( );
    auto labelMax = ImGui::GetItemRectMax( );
    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::Dummy( ImVec2( 0.0, frameHeight + itemSpacing.y ) );
    ImGui::BeginGroup( );

    // ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

    ImGui::PopStyleVar( 2 );

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow( )->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->WorkRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow( )->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow( )->Size.x -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth( );
    ImGui::PushItemWidth( ImMax( 0.0f, itemWidth - frameHeight ) );

    s_GroupPanelLabelStack.push_back( ImRect( labelMin, labelMax ) );
}

void
EndGroupPanel( )
{
    ImGui::PopItemWidth( );

    auto itemSpacing = ImGui::GetStyle( ).ItemSpacing;

    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );

    auto frameHeight = ImGui::GetFrameHeight( );

    ImGui::EndGroup( );

    // ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup( );

    ImGui::SameLine( 0.0f, 0.0f );
    ImGui::Dummy( ImVec2( frameHeight * 0.5f, 0.0f ) );
    ImGui::Dummy( ImVec2( 0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y ) );

    ImGui::EndGroup( );

    auto itemMin = ImGui::GetItemRectMin( );
    auto itemMax = ImGui::GetItemRectMax( );
    // ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

    auto labelRect = s_GroupPanelLabelStack.back( );
    s_GroupPanelLabelStack.pop_back( );

    ImVec2 halfFrame = ImVec2( frameHeight * 0.25f, frameHeight ) * 0.5f;
    ImRect frameRect = ImRect( itemMin + halfFrame, itemMax - ImVec2( halfFrame.x, 0.0f ) );
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for ( int i = 0; i < 4; ++i )
    {
        switch ( i )
        {
        // left half-plane
        case 0: ImGui::PushClipRect( ImVec2( -FLT_MAX, -FLT_MAX ), ImVec2( labelRect.Min.x, FLT_MAX ), true ); break;
        // right half-plane
        case 1: ImGui::PushClipRect( ImVec2( labelRect.Max.x, -FLT_MAX ), ImVec2( FLT_MAX, FLT_MAX ), true ); break;
        // top
        case 2: ImGui::PushClipRect( ImVec2( labelRect.Min.x, -FLT_MAX ), ImVec2( labelRect.Max.x, labelRect.Min.y ), true ); break;
        // bottom
        case 3: ImGui::PushClipRect( ImVec2( labelRect.Min.x, labelRect.Max.y ), ImVec2( labelRect.Max.x, FLT_MAX ), true ); break;
        }

        ImGui::GetWindowDrawList( )->AddRect(
            frameRect.Min, frameRect.Max,
            ImColor( ImGui::GetStyleColorVec4( ImGuiCol_Border ) ),
            halfFrame.x );

        ImGui::PopClipRect( );
    }

    ImGui::PopStyleVar( 2 );

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow( )->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->WorkRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow( )->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow( )->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow( )->Size.x += frameHeight;

    ImGui::Dummy( ImVec2( 0.0f, 0.0f ) );

    ImGui::EndGroup( );
}
}   // namespace

void
ToImGuiWidget( const char* Name, uint32_t* Value )
{
    ImGui::InputScalar( Name, ImGuiDataType_S32, Value );
}

void
ToImGuiWidget( const char* Name, std::shared_ptr<Shader>* Value )
{
    auto PtrAddress = reinterpret_cast<uint64_t>( Value->get( ) );
    ImGui::BeginDisabled( );
    ImGui::InputScalar( Name, ImGuiDataType_U64, &PtrAddress, nullptr, nullptr, "%p", ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal );
    ImGui::EndDisabled( );
}

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

        if ( m_ConceptInspectionCache.SelectedConcept != nullptr )
        {
            const auto* Name = m_ConceptInspectionCache.SelectedConcept->GetName( );

            if ( ImGui::BeginTabBar( Name ) )
            {
                if ( ImGui::BeginTabItem( Name ) )
                {
                    if ( m_ConceptInspectionCache.SelectedConcept->CanCastV( Sprite::TypeID ) )
                    {
                        ::BeginGroupPanel( "Sprite", ImVec2 { -1, 0 } );

                        ToImGuiWidget( "Sprite", (Sprite*) m_ConceptInspectionCache.SelectedConcept );

                        ImGui::Spacing( );
                        ::EndGroupPanel( );
                    }

                    ImGui::EndTabItem( );
                }

                if ( ImGui::BeginTabItem( "Memory" ) )
                {
                    ConceptMemoryViewGroup( "Memory View", m_ConceptInspectionCache.SelectedConcept );
                    ImGui::EndTabItem( );
                }

                ImGui::EndTabBar( );
            }
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
            m_ConceptInspectionCache.SelectedConcept = Con;
        }

        if ( Con->CanCastV( Concept::TypeID ) && ( (Concept*) Con )->HasSubConcept( ) )
        {
            // Has sub node
            bool node_open = ImGui::TreeNodeEx( Con, BaseFlag, "%s", Con->GetName( ) );
            IsSelected     = ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( );

            DragAndDropOperation( Con );

            if ( node_open )
            {
                auto& ConceptsCache = m_ConceptInspectionCache.ConceptTree[ Con->GetHash( ) ];
                ( (Concept*) Con )->GetConcepts( ConceptsCache );

                ConceptsCache.ForEachIndex( [ this ]( auto Index, PureConcept* Con ) {
                    ImGui::PushID( Con );

                    if ( RenderConceptHierarchy( Con ) )
                    {
                        m_ConceptInspectionCache.SelectedConceptMask.clear( );
                        m_ConceptInspectionCache.SelectedConceptMask.emplace( Con );
                    }

                    ImGui::PopID( );
                } );

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

void
EditorWindow::ConceptMemoryViewGroup( const char* Name, PureConcept* Con )
{
    ::BeginGroupPanel( Name, ImVec2 { -1, 0 } );

    static MemoryEditor mem_edit;
    mem_edit.DrawContents( Con,
                           Con->GetSizeofV( ),
                           (size_t) Con );

    ImGui::Spacing( );

    ::EndGroupPanel( );
}

void
EditorWindow::SetRootConcept( DynamicConcept* RootConcept )
{
    GameWindow::SetRootConcept( RootConcept );

    m_ConceptInspectionCache = { };
}
