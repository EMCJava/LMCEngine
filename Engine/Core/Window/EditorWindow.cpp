//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <libexecstream/exec-stream.h>

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/File/OSFile.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Engine.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <ImGui/ImGui.hpp>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <filesystem>
#include <regex>

namespace
{

bool
CopyProjectAssets( const std::filesystem::path& Path, std::filesystem::path Destination )
{
    const auto CopyOptions =
        std::filesystem::copy_options::update_existing
        | std::filesystem::copy_options::recursive;

    if ( !std::filesystem::exists( Path ) )
    {
        spdlog::warn( "Path does not exist, not copying: {}", absolute( Path ).string( ) );
        return false;
    }

    const bool IsDirectory = std::filesystem::is_directory( Path );

    std::error_code OperationError;
    spdlog::info( "Copying {} {} to {}", IsDirectory ? "directory" : "file", absolute( Path ).string( ), absolute( Destination ).string( ) );
    if ( IsDirectory )
    {
        const auto DirectoryName = Path.filename( );
        Destination              = Destination / DirectoryName;
        std::filesystem::create_directories( Destination, OperationError );

        if ( OperationError.value( ) )
        {
            spdlog::error( "Failed to create directory at destination: {}({})", OperationError.message( ), OperationError.value( ) );
            return false;
        }
    }

    std::filesystem::copy( Path, Destination, CopyOptions, OperationError );

    constexpr size_t RetryCount = 5;
    for ( size_t Tries = 0; Tries < RetryCount && OperationError.value( ); ++Tries )
    {
        spdlog::error( "Failed, {}({}), retry left: {}", OperationError.message( ), OperationError.value( ), RetryCount - Tries );
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        std::filesystem::copy( Path, Destination, CopyOptions, OperationError );
    }

    if ( OperationError.value( ) )
    {
        spdlog::error( "Failed, {}({}), unable to copy", OperationError.message( ), OperationError.value( ) );

        return false;
    }

    return true;
}

int
RunCommand( const std::string& Program, const std::string& Arguments, auto&& OnStdOut, auto&& OnStdErr, const std::vector<std::pair<std::string, std::string>>& PreCommands = { } )
{
    exec_stream_t es;
    es.set_wait_timeout( exec_stream_t::stream_kind_t::s_all, 10 * 60 * 1000 );

    for ( const auto& Cmd : PreCommands )
    {
        spdlog::info( "Running command: {} {}", Cmd.first, Cmd.second );
        es.start( Cmd.first, Cmd.second );
        std::string s;
        while ( std::getline( es.out( ), s ) )
        {
            spdlog::info( "exec_stream(out) : - {} -", s );
        }
        while ( std::getline( es.err( ), s ) )
        {
            spdlog::info( "exec_stream(err) : - {} -", s );
        }
        es.close( );
    }

    spdlog::info( "Running command: {} {}", Program, Arguments );
    es.start( Program, Arguments );

    auto StdErr = std::thread( [ &es, &OnStdErr ]( ) {
        std::string s;
        while ( std::getline( es.err( ), s ) )
        {
            OnStdErr( s );
        }
    } );

    auto StdOut = std::thread( [ &es, &OnStdOut ]( ) {
        std::string s;
        while ( std::getline( es.out( ), s ) )
        {
            OnStdOut( s );
        }
    } );

    if ( StdErr.joinable( ) ) StdErr.join( );
    if ( StdOut.joinable( ) ) StdOut.join( );

    es.close( );
    return es.exit_code( );
}
}   // namespace

// From ImGiu demo
struct LogGroup {
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int>   LineOffsets;   // Index to lines offset. We maintain this with AddLog() calls.
    bool            AutoScroll;    // Keep scrolling if already at the bottom.

    LogGroup( )
    {
        AutoScroll = true;
        Clear( );
    }

    void Clear( )
    {
        Buf.clear( );
        LineOffsets.clear( );
        LineOffsets.push_back( 0 );
    }

    void AddLog( const char* fmt, ... ) IM_FMTARGS( 2 )
    {
        int     old_size = Buf.size( );
        va_list args;
        va_start( args, fmt );
        Buf.appendfv( fmt, args );
        va_end( args );
        for ( int new_size = Buf.size( ); old_size < new_size; old_size++ )
            if ( Buf[ old_size ] == '\n' )
                LineOffsets.push_back( old_size + 1 );
    }

    void Draw( const char* title, bool* p_open = nullptr, float Height = 0 )
    {
        ImGuiGroup::BeginGroupPanel( title );

        // Options menu
        if ( ImGui::BeginPopup( "Options" ) )
        {
            ImGui::Checkbox( "Auto-scroll", &AutoScroll );
            ImGui::EndPopup( );
        }

        // Main window
        if ( ImGui::Button( "Options" ) )
            ImGui::OpenPopup( "Options" );
        ImGui::SameLine( );
        bool clear = ImGui::Button( "Clear" );
        ImGui::SameLine( );
        bool copy = ImGui::Button( "Copy" );
        ImGui::SameLine( );
        Filter.Draw( "Filter", -100.0f );


        ImGui::Separator( );

        if ( ImGui::BeginChild( "scrolling", ImVec2( 0, Height ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
        {
            if ( clear )
                Clear( );
            if ( copy )
                ImGui::LogToClipboard( );

            ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
            const char* buf     = Buf.begin( );
            const char* buf_end = Buf.end( );
            if ( Filter.IsActive( ) )
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for ( int line_no = 0; line_no < LineOffsets.Size; line_no++ )
                {
                    const char* line_start = buf + LineOffsets[ line_no ];
                    const char* line_end   = ( line_no + 1 < LineOffsets.Size ) ? ( buf + LineOffsets[ line_no + 1 ] - 1 ) : buf_end;
                    if ( Filter.PassFilter( line_start, line_end ) )
                        ImGui::TextUnformatted( line_start, line_end );
                }
            } else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                // within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                // on your side is recommended. Using ImGuiListClipper requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display
                // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                // it possible (and would be recommended if you want to search through tens of thousands of entries).
                ImGuiListClipper clipper;
                clipper.Begin( LineOffsets.Size );
                while ( clipper.Step( ) )
                {
                    for ( int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++ )
                    {
                        const char* line_start = buf + LineOffsets[ line_no ];
                        const char* line_end   = ( line_no + 1 < LineOffsets.Size ) ? ( buf + LineOffsets[ line_no + 1 ] - 1 ) : buf_end;
                        ImGui::TextUnformatted( line_start, line_end );
                    }
                }
                clipper.End( );
            }
            ImGui::PopStyleVar( );

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if ( AutoScroll && ImGui::GetScrollY( ) >= ImGui::GetScrollMaxY( ) )
                ImGui::SetScrollHereY( 1.0f );
        }
        ImGui::EndChild( );

        ImGuiGroup::EndGroupPanel( );
    }
};

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

    m_BuildThreadStrBuffer = std::make_unique<LogGroup>( );
}

EditorWindow::EditorWindow( const char* Title, bool Create )
    : GameWindow( Title, Create )
{
    m_HRFrameBuffer = new HotReloadFrameBuffer;
    m_HRFrameBuffer->SetGLContext( m_GLContext );
    m_HRFrameBuffer->CreateFrameBuffer( m_Width, m_Height );

    m_BuildThreadStrBuffer = std::make_unique<LogGroup>( );
}

EditorWindow::~EditorWindow( )
{
    m_ShouldBuild = false;
    if ( m_BuildThread && m_BuildThread->joinable( ) )
    {
        m_BuildThread->join( );
        m_BuildThread.reset( );
    }

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

    std::optional<bool> ShowProjectBuild;

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
                ShowProjectBuild = true;
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
                    const auto ToImGuiFuncPtr = Engine::GetEngine( )->GetConceptToImGuiFuncPtr( m_ConceptInspectionCache.SelectedConcept->GetTypeIDV( ) );
                    if ( ToImGuiFuncPtr != nullptr )
                    {
                        ImGuiGroup::BeginGroupPanel( "Property inspect", ImVec2 { -1, 0 } );

                        ToImGuiFuncPtr( Name, m_ConceptInspectionCache.SelectedConcept );

                        ImGui::Spacing( );
                        ImGuiGroup::EndGroupPanel( );
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

    {
        if ( ShowProjectBuild.has_value( ) )
        {
            if ( ShowProjectBuild.value( ) )
            {
                SetBuildPath( );

                m_BuildThreadAllowed = m_MaxThreadAllowed = std::thread::hardware_concurrency( );
                ImGui::OpenPopup( "Project Build" );
            }
        }

        const ImVec2 Center = ImGui::GetMainViewport( )->GetCenter( );
        ImGui::SetNextWindowPos( Center, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );
        if ( ImGui::BeginPopup( "Project Build", ImGuiWindowFlags_AlwaysAutoResize ) )
        {
            {
                if ( m_BuildFailedAt == BuildStage::CMakeConfig )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                }

                ImGuiGroup::BeginGroupPanel( "Cmake Settings" );

                ImGui::Text( "%s", m_BuildPath.string( ).c_str( ) );
                ImGui::SameLine( );
                if ( ImGui::ArrowButton( "##PF", ImGuiDir_Down ) )
                {
                    SetBuildPath( );
                }

                ImGui::Spacing( );
                ImGuiGroup::EndGroupPanel( );

                if ( m_BuildFailedAt == BuildStage::CMakeConfig )
                {
                    ImGui::PopStyleColor( );
                }
            }

            ImGui::Spacing( );

            {
                if ( m_BuildFailedAt == BuildStage::CmakeBuild )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                }

                ImGuiGroup::BeginGroupPanel( "Build Settings" );

                const uint32_t MinThread = 1;
                ImGui::SliderScalar( "Build Threads", ImGuiDataType_U32, &m_BuildThreadAllowed, &MinThread, &m_MaxThreadAllowed );

                ImGui::Spacing( );
                ImGuiGroup::EndGroupPanel( );

                if ( m_BuildFailedAt == BuildStage::CmakeBuild )
                {
                    ImGui::PopStyleColor( );
                }
            }

            {
                if ( m_BuildFailedAt == BuildStage::CopyFiles )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
                }

                ImGuiGroup::BeginGroupPanel( "Assets Settings" );

                ImGui::SeparatorText( "Copy Asset Files To" );
                ImGui::BeginDisabled( );
                ImGui::Text( "%s", m_BuildPath.string( ).c_str( ) );
                ImGui::EndDisabled( );

                ImGui::Spacing( );
                ImGuiGroup::EndGroupPanel( );

                if ( m_BuildFailedAt == BuildStage::CopyFiles )
                {
                    ImGui::PopStyleColor( );
                }
            }

            if ( ImGui::Button( "Build", ImVec2( 120, 0 ) ) )
            {
                ImGui::OpenPopup( "Build Progress" );

                std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                m_BuildThreadStrBuffer->Clear( );

                m_BuildFailedAt = BuildStage::None;
                m_BuildStage    = BuildStage::CMakeConfig;
                m_BuildThread   = std::make_unique<std::thread>( &EditorWindow::BuildReleaseConfigCmake, this );
            }
            ImGui::SetItemDefaultFocus( );
            ImGui::SameLine( );
            if ( ImGui::Button( "Finish", ImVec2( 120, 0 ) ) )
            {
                ImGui::CloseCurrentPopup( );
            }

            /*
             *
             * Build progress bar
             *
             * */
            {
                ImGui::SetNextWindowPos( Center, ImGuiCond_Always, ImVec2( 0.5f, 0.5f ) );
                ImGui::SetNextWindowSize( ImVec2 { 0, 0 } );

                if ( ImGui::BeginPopupModal( "Build Progress", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
                {

                    float       progress = 0;
                    std::string StatusStr;
                    if ( m_BuildThread == nullptr )
                    {
                        progress = 1;

                        // Finished
                        if ( m_BuildStage == BuildStage::None || m_BuildFailedAt != BuildStage::None )
                        {

                            StatusStr = m_BuildFailedAt != BuildStage::None ? "Failed Building" : "Finishing Building";
                        } else
                        {
                            // Failed
                            if ( m_BuildStage != BuildStage::Finished )
                            {
                                m_BuildFailedAt = m_BuildStage;
                            }

                            m_BuildStage = BuildStage::None;
                        }
                    } else
                    {
                        switch ( m_BuildStage )
                        {
                        case BuildStage::CMakeConfig:
                            progress  = 1 / 4.F;
                            StatusStr = "Cmake Configuration";
                            break;
                        case BuildStage::CmakeBuild:
                            progress  = 2 / 4.F;
                            StatusStr = "Project Building";
                            break;
                        case BuildStage::CopyFiles:
                            progress  = 3 / 4.F;
                            StatusStr = "Copying Project Asset Files";
                            break;
                        case BuildStage::Finished:
                        case BuildStage::None:
                            break;
                        }
                    }

                    ImGui::ProgressBar( progress, ImVec2( m_Width / 2.F, 0.f ), StatusStr.c_str( ) );

                    // Not finished, show build messages
                    if ( m_BuildStage != BuildStage::Finished )
                    {
                        std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                        m_BuildThreadStrBuffer->Draw( "Build Logs", nullptr, m_Height / 2.F );
                    }

                    ImGui::Separator( );
                    ImGui::Dummy( ImVec2( 120 /*+ ImGui::GetStyle( ).ItemSpacing.x*/, 0 ) );
                    ImGui::Spacing( );

                    if ( m_BuildStage != BuildStage::None )
                    {
                        ImGui::BeginDisabled( );
                    }

                    if ( ImGui::Button( "Close", ImVec2( 120, 0 ) ) )
                    {
                        ImGui::CloseCurrentPopup( );
                    }

                    if ( m_BuildStage != BuildStage::None )
                    {
                        ImGui::EndDisabled( );
                    }

                    ImGui::EndPopup( );
                }
            }

            ImGui::EndPopup( );
        }
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

                ConceptsCache.ForEachIndex( [ this ]( auto Index, std::shared_ptr<PureConcept>& Con ) {
                    ImGui::PushID( Con.get( ) );

                    if ( RenderConceptHierarchy( Con.get( ) ) )
                    {
                        m_ConceptInspectionCache.SelectedConceptMask.clear( );
                        m_ConceptInspectionCache.SelectedConceptMask.emplace( Con.get( ) );
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
    ImGuiGroup::BeginGroupPanel( Name, ImVec2 { -1, 0 } );

    static MemoryEditor mem_edit;
    mem_edit.DrawContents( Con,
                           Con->GetSizeofV( ),
                           (size_t) Con );

    ImGui::Spacing( );

    ImGuiGroup::EndGroupPanel( );
}

void
EditorWindow::SetRootConcept( RootConceptTy* RootConcept )
{
    GameWindow::SetRootConcept( RootConcept );

    m_ConceptInspectionCache = { };
}

void
EditorWindow::SetBuildPath( )
{
    REQUIRED_IF( m_BuildStage == BuildStage::None )
    {
        const auto&           Project           = Engine::GetEngine( )->GetProject( );
        std::filesystem::path ProjectFolderPath = Project->GetProjectPath( );
        if ( !ProjectFolderPath.empty( ) )
        {
            ProjectFolderPath      = ProjectFolderPath.parent_path( );
            const auto defaultPath = m_BuildPath;
            m_BuildPath            = OSFile::PickFolder( ProjectFolderPath.string( ).c_str( ) );
            if ( m_BuildPath.empty( ) )
            {
                m_BuildPath = defaultPath;
            }
            spdlog::info( "Building project {} in path: {}", Project->GetConfig( ).project_name, m_BuildPath.string( ) );
        }
    }
}

void
EditorWindow::BuildReleaseConfigCmake( )
{
    REQUIRED_IF( !m_BuildPath.empty( ) && m_BuildStage == BuildStage::CMakeConfig && m_BuildThread != nullptr && std::this_thread::get_id( ) == m_BuildThread->get_id( ) )
    {
        const auto&           Project           = Engine::GetEngine( )->GetProject( );
        std::filesystem::path ProjectFolderPath = Project->GetProjectPath( );
        ProjectFolderPath                       = ProjectFolderPath.parent_path( );

        /*
         *
         * Cmake setup
         *
         * */
        try
        {
            std::string Arguments = ProjectFolderPath.string( ) /* + " -G Ninja"*/ + " -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=" + m_BuildPath.string( ) + " -DEditorBuild=false" + " -B " + ( m_BuildPath / "cmake" ).string( );
            Arguments             = std::regex_replace( Arguments, std::regex( R"(\\)" ), "/" );   // Why????

            std::string ErrorLogs       = "";
            float       ConfiguringTime = 0, GeneratingTime = 0;

            RunCommand(
                "cmake", Arguments,
                [ this, &ConfiguringTime, &GeneratingTime ]( auto& s ) {
                    spdlog::info( "exec_stream : - {} -", s );
                    std::regex  GeneratingPattern( R"(Generating done \((\d+\.\d+)s\))" );
                    std::smatch GeneratingMatch;
                    if ( std::regex_search( s, GeneratingMatch, GeneratingPattern ) )
                    {
                        GeneratingTime = std::stof( GeneratingMatch.str( 1 ) );
                    }

                    std::regex  ConfiguringPattern( R"(Configuring done \((\d+\.\d+)s\))" );
                    std::smatch ConfiguringMatch;
                    if ( std::regex_search( s, ConfiguringMatch, ConfiguringPattern ) )
                    {
                        ConfiguringTime = std::stof( ConfiguringMatch.str( 1 ) );
                    }

                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", s.c_str( ) );
                },
                [ this, &ErrorLogs ]( auto& s ) {
                    ErrorLogs += s + '\n';
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", s.c_str( ) );
                } );

            if ( GeneratingTime != 0 )
            {
                spdlog::info( "Cmake generation complete, CT: {}s, GT: {}s", ConfiguringTime, GeneratingTime );

                {
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->Clear( );
                }

                if ( m_ShouldBuild )
                {
                    m_BuildThread->detach( );
                    m_BuildStage  = BuildStage::CmakeBuild;
                    m_BuildThread = std::make_unique<std::thread>( &EditorWindow::BuildRelease, this );
                }
            } else
            {
                spdlog::error( "Cmake generation failed" );
                spdlog::error( "{}", ErrorLogs );

                m_BuildThread->detach( );
                m_BuildThread.reset( );
            }
        }
        catch ( std::exception const& e )
        {
            spdlog::error( "error: {}", e.what( ) );

            m_BuildThread->detach( );
            m_BuildThread.reset( );
        }
    }
}

void
EditorWindow::BuildRelease( )
{
    REQUIRED_IF( !m_BuildPath.empty( ) && m_BuildStage == BuildStage::CmakeBuild && m_BuildThread != nullptr && std::this_thread::get_id( ) == m_BuildThread->get_id( ) )
    {
        /*
         *
         * Build
         *
         * */
        try
        {
            std::string Arguments = "--build " + ( m_BuildPath / "cmake" ).string( ) + " -j " + std::to_string( m_BuildThreadAllowed );
            Arguments             = std::regex_replace( Arguments, std::regex( R"(\\)" ), "/" );   // Why????

            std::string ErrorLogs = "";
            const auto  ExitCode  = RunCommand(
                "cmake", Arguments,
                [ this ]( auto& s ) {
                    spdlog::info( "exec_stream : - {} -", s );
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", s.c_str( ) );
                },
                [ this, &ErrorLogs ]( auto& s ) {
                    ErrorLogs += s + '\n';
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", s.c_str( ) );
                } );

            if ( ExitCode == 0 )
            {
                {
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->Clear( );
                }

                if ( m_ShouldBuild )
                {
                    m_BuildThread->detach( );
                    m_BuildStage  = BuildStage::CopyFiles;
                    m_BuildThread = std::make_unique<std::thread>( &EditorWindow::BuildReleaseCopyEssentialFiles, this );
                }
            } else
            {
                spdlog::error( "Build failed: - {} -", ErrorLogs );
                m_BuildThread->detach( );
                m_BuildThread.reset( );
            };
        }
        catch ( std::exception const& e )
        {
            spdlog::error( "error: {}", e.what( ) );

            m_BuildThread->detach( );
            m_BuildThread.reset( );
        }
    }
}

void
EditorWindow::BuildReleaseCopyEssentialFiles( )
{
    REQUIRED_IF( !m_BuildPath.empty( ) && m_BuildStage == BuildStage::CopyFiles && m_BuildThread != nullptr && std::this_thread::get_id( ) == m_BuildThread->get_id( ) )
    {
        const auto&           Project           = Engine::GetEngine( )->GetProject( );
        std::filesystem::path ProjectFolderPath = Project->GetProjectPath( );
        ProjectFolderPath                       = ProjectFolderPath.parent_path( );

        /*
         *
         * Copy essential project files
         *
         * */
        std::string ProjectPaths[] = { "Assets", "Editor" };
        for ( const auto& Path : ProjectPaths )
        {
            if ( !CopyProjectAssets( ProjectFolderPath / Path, m_BuildPath ) )
            {
                m_BuildThread->detach( );
                m_BuildThread.reset( );
                return;
            }
        }

        if ( !CopyProjectAssets( Project->GetProjectPath( ), m_BuildPath ) )
        {
            m_BuildThread->detach( );
            m_BuildThread.reset( );
            return;
        }

        m_BuildStage = BuildStage::Finished;
        m_BuildThread->detach( );
        m_BuildThread.reset( );
    }
}
