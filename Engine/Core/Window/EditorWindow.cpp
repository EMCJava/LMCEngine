//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Runtime/ExternalProgram/ProgramExecute.hpp>
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
#include <ImGui/Ext/LogGroup.hpp>
#include <imgui_memory_editor/imgui_memory_editor.h>

#include <filesystem>
#include <regex>

void
EditorWindow::Update( )
{
    m_HRFrameBuffer->BindFrameBuffer( );
    const auto MainWindowViewPortDimensions = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );
    if ( m_FrameBufferDimension != MainWindowViewPortDimensions )
    {
        m_FrameBufferDimension = MainWindowViewPortDimensions;
        // Size changed, need to recreate the frame buffer
        m_HRFrameBuffer->RescaleFrameBuffer( m_FrameBufferDimension.first, m_FrameBufferDimension.second );
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

    ImGui::DockSpaceOverViewport( ImGui::GetMainViewport( ) );

    if ( m_ShowImGuiDemoWindow )
    {
        ImGui::ShowDemoWindow( );
    }

    RenderMenuBar( );

    {
        ImGui::Begin( "Hierarchy" );

        m_ConceptDragStart = m_ConceptDragEnd = nullptr;

        auto RootConcept = GetConceptFakeSharedPtr( );
        if ( RenderConceptHierarchy( RootConcept ) )
        {
            m_ConceptInspectionCache.SelectedConceptMask.clear( );
            m_ConceptInspectionCache.SelectedConceptMask.emplace( RootConcept.get( ) );
        }

        /*
         *
         * Drag and drop operations
         *
         * */
        {
            if ( m_ConceptDragStart != nullptr && m_ConceptDragEnd != nullptr )
            {
                if ( m_ConceptDragEnd->CanCastV( Concept::TypeID ) ) [[likely]]
                {
                    ( (Concept*) m_ConceptDragEnd )->TransferSubConcept( m_ConceptDragStart );
                    spdlog::info( "Transferred subConcept ownership" );
                }
            }
        }

        ImGui::End( );
    }

    {
        ImGui::Begin( "Console" );
        Engine::GetEngine( )->GetEngineDefaultLogGroup( )->Draw( "Engine Log", false );
        ImGui::End( );
    }

    {
        ImGui::Begin( "Details" );

        if ( !m_ConceptInspectionCache.SelectedConcept.expired( ) )
        {
            auto        SelectedConcept = m_ConceptInspectionCache.SelectedConcept.lock( );
            const auto* Name            = SelectedConcept->GetName( );

            if ( ImGui::BeginTabBar( Name ) )
            {
                if ( ImGui::BeginTabItem( Name ) )
                {
                    const auto ToImGuiFuncPtr = Engine::GetEngine( )->GetConceptToImGuiFuncPtr( SelectedConcept->GetTypeIDV( ) );
                    if ( ToImGuiFuncPtr != nullptr )
                    {
                        ImGuiGroup::BeginGroupPanel( "Property inspect", ImVec2 { -1, 0 } );

                        ToImGuiFuncPtr( Name, SelectedConcept.get( ) );

                        ImGui::Spacing( );
                        ImGuiGroup::EndGroupPanel( );
                    }

                    ImGui::EndTabItem( );
                }

                if ( ImGui::BeginTabItem( "Memory" ) )
                {
                    ConceptMemoryViewGroup( "Memory View", SelectedConcept.get( ) );
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
        auto ChildStartPosition = ImGui::GetWindowPos( );

        ImVec2      ChildStartPos = ImGui::GetCursorScreenPos( );
        const auto& io            = ImGui::GetIO( );

        const auto                WindowDimensions    = ImGui::GetContentRegionAvail( );
        const std::pair<int, int> WindowDimensionPair = { WindowDimensions.x, WindowDimensions.y };
        if ( m_ViewportPhysicalDimension != WindowDimensionPair )
        {
            Engine::GetEngine( )->SetPhysicalMainWindowViewPortDimensions( m_ViewportPhysicalDimension = WindowDimensionPair );
        }

        const auto ImageSize = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );

        const FloatTy XOffset = ( WindowDimensions.x - ImageSize.first ) * 0.5f;
        const FloatTy YOffset = ( WindowDimensions.y - ImageSize.second ) * 0.5f;

        ImGui::SetCursorPosX( XOffset );
        ImGui::SetCursorPosY( YOffset );

        Engine::GetEngine( )->GetUserInputHandle( )->SetCursorTopLeftPosition( { ChildStartPosition.x + XOffset, ChildStartPosition.y + YOffset } );

        ImGui::Image( reinterpret_cast<void*>( m_PreviousFrameTextureID ),
                      ImVec2( ImageSize.first, ImageSize.second ), ImVec2( 0, 1 ), ImVec2( 1, 0 ) );

        {
            ImGui::SetCursorScreenPos( ChildStartPos );
            const ImGuiWindowFlags OverlayFlag = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            ImGui::SetNextWindowBgAlpha( 0.35f );   // Transparent background
            ImGui::SetNextWindowPos( ChildStartPosition, ImGuiCond_Always );
            if ( ImGui::Begin( "Example: Simple overlay", nullptr, OverlayFlag ) )
            {
                ImGui::Text( "%.1f FPS", io.Framerate );
            }
            ImGui::End( );
        }

        ImGui::EndChild( );
        ImGui::End( );
    }

    /*
     *
     * Project build overlay
     *
     * */
    {
        if ( m_ShouldActivateOverlay )
        {
            SetBuildPath( );

            m_BuildThreadAllowed = m_MaxThreadAllowed = std::thread::hardware_concurrency( );
            ImGui::OpenPopup( "Project Build" );
            m_ShouldActivateOverlay = false;
        }

        RenderBuildOverlay( );
    }
}

bool
EditorWindow::RenderConceptHierarchy( std::shared_ptr<PureConcept>& ConShared )
{
    bool IsSelected = false;

    static const auto DragAndDropOperation = [ this ]( std::shared_ptr<PureConcept>& ConShared ) {
        /*
         *
         * Drag and drop operations
         *
         * */
        if ( ImGui::BeginDragDropSource( ) )
        {
            auto* ConPtr = ConShared.get( );
            ImGui::SetDragDropPayload( "ConceptDAD", &ConPtr, sizeof( PureConcept* ) );
            ImGui::Text( "Moves as sub-concept" );
            ImGui::EndDragDropSource( );
        }

        if ( ImGui::BeginDragDropTarget( ) )
        {
            if ( const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload( "ConceptDAD" ) )
            {
                PureConcept* PayloadConcept = *( (PureConcept**) Payload->Data );
                spdlog::info( "Drag concept from {} to {}", PayloadConcept->GetName( ), ConShared->GetName( ) );
                m_ConceptDragStart = PayloadConcept;
                m_ConceptDragEnd   = ConShared.get( );
            }
            ImGui::EndDragDropTarget( );
        }
    };

    if ( ConShared != nullptr )
    {
        ImGuiTreeNodeFlags BaseFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        if ( m_ConceptInspectionCache.SelectedConceptMask.contains( ConShared.get( ) ) )
        {
            BaseFlag |= ImGuiTreeNodeFlags_Selected;
            m_ConceptInspectionCache.SelectedConcept = ConShared;
        }

        if ( ConShared->CanCastV( Concept::TypeID ) && ( (Concept*) ConShared.get( ) )->HasSubConcept( ) )
        {
            // Has sub node
            const bool Enabled = ConShared->IsEnabled( );

            if ( !Enabled ) ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle( ).Alpha * ImGui::GetStyle( ).DisabledAlpha );
            bool node_open = ImGui::TreeNodeEx( ConShared.get( ), BaseFlag, "%s", ConShared->GetName( ) );
            if ( !Enabled ) ImGui::PopStyleVar( );

            IsSelected = ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( );

            DragAndDropOperation( ConShared );

            if ( node_open )
            {
                ( (Concept*) ConShared.get( ) )->ForEachSubConcept( [ this ]( std::shared_ptr<PureConcept>& ConShared ) {
                    ImGui::PushID( ConShared.get( ) );

                    if ( RenderConceptHierarchy( ConShared ) )
                    {
                        m_ConceptInspectionCache.SelectedConceptMask.clear( );
                        m_ConceptInspectionCache.SelectedConceptMask.emplace( ConShared.get( ) );
                    }

                    ImGui::PopID( );
                } );

                ImGui::TreePop( );
            }
        } else
        {
            // No sub node
            BaseFlag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            const bool Enabled = ConShared->IsEnabled( );

            if ( !Enabled ) ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle( ).Alpha * ImGui::GetStyle( ).DisabledAlpha );
            ImGui::TreeNodeEx( ConShared.get( ), BaseFlag, "%s", ConShared->GetName( ) );
            if ( !Enabled ) ImGui::PopStyleVar( );

            IsSelected = ImGui::IsItemClicked( ) && !ImGui::IsItemToggledOpen( );

            DragAndDropOperation( ConShared );
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
    m_FrameBufferDimension   = { };
}

void
EditorWindow::SetBuildPath( )
{
    REQUIRED_IF( m_BuildStage == BuildStage::None )
    {
        const auto&           Project           = Engine::GetEngine( )->GetProject( );
        std::filesystem::path ProjectFolderPath = Project->GetProjectFilePath( );
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
        std::filesystem::path ProjectFolderPath = Project->GetProjectFilePath( );
        ProjectFolderPath                       = ProjectFolderPath.parent_path( );

        /*
         *
         * Cmake setup
         *
         * */
        try
        {
            std::string Arguments = ProjectFolderPath.string( ) /* + " -G Ninja"*/ + " -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + m_BuildPath.string( ) + " -DEditorBuild=false" + " -B " + ( m_BuildPath / "cmake" ).string( );
            if ( !m_PreferredCompiler.empty( ) ) Arguments += +" -DCMAKE_CXX_COMPILER=" + m_PreferredCompiler + " ";
            Arguments = std::regex_replace( Arguments, std::regex( R"(\\)" ), "/" );   // Why????

            std::string ErrorLogs       = "";
            float       ConfiguringTime = 0, GeneratingTime = 0;

            ExternalProgram::RunCommand(
                "cmake", Arguments,
                [ this, &ConfiguringTime, &GeneratingTime ]( const auto& CommandLog ) {
                    spdlog::info( "exec_stream : - {} -", CommandLog );
                    std::regex  GeneratingPattern( R"(Generating done \((\d+\.\d+)s\))" );
                    std::smatch GeneratingMatch;
                    if ( std::regex_search( CommandLog, GeneratingMatch, GeneratingPattern ) )
                    {
                        GeneratingTime = std::stof( GeneratingMatch.str( 1 ) );
                    }

                    std::regex  ConfiguringPattern( R"(Configuring done \((\d+\.\d+)s\))" );
                    std::smatch ConfiguringMatch;
                    if ( std::regex_search( CommandLog, ConfiguringMatch, ConfiguringPattern ) )
                    {
                        ConfiguringTime = std::stof( ConfiguringMatch.str( 1 ) );
                    }

                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", CommandLog.c_str( ) );
                },
                [ this, &ErrorLogs ]( const auto& CommandLog ) {
                    ErrorLogs += CommandLog + '\n';
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", CommandLog.c_str( ) );
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
            const auto  ExitCode  = ExternalProgram::RunCommand(
                "cmake", Arguments,
                [ this ]( const auto& CommandLog ) {
                    spdlog::info( "exec_stream : - {} -", CommandLog );
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", CommandLog.c_str( ) );
                },
                [ this, &ErrorLogs ]( const auto& CommandLog ) {
                    ErrorLogs += CommandLog + '\n';
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->AddLog( "%s\n", CommandLog.c_str( ) );
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
        std::filesystem::path ProjectFolderPath = Project->GetProjectFilePath( );
        ProjectFolderPath                       = ProjectFolderPath.parent_path( );

        /*
         *
         * Copy essential project files
         *
         * */
        std::string ProjectPaths[] = { "Assets", "Editor" };
        for ( const auto& Path : ProjectPaths )
        {
            if ( !OSFile::CopyFolder( ProjectFolderPath / Path, m_BuildPath ) )
            {
                m_BuildThread->detach( );
                m_BuildThread.reset( );
                return;
            }
        }

        if ( !OSFile::CopyFolder( Project->GetProjectFilePath( ), m_BuildPath ) )
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

void
EditorWindow::RenderBuildOverlay( )
{
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

            ToImGuiWidget( "Preferred Compiler", &m_PreferredCompiler );

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
                        } else
                        {
#ifdef LMC_WIN
                            std::string OpenFolderSysCommand = "explorer " + m_BuildPath.string( );
#elif defined( LMC_APPLE )
                            std::string OpenFolderSysCommand = "open " + m_BuildPath.string( );
#elif defined( LMC_LINUX )
                            std::string OpenFolderSysCommand = "xdg-open " + m_BuildPath.string( );
#else
                            std::string OpenFolderSysCommand = "";
#endif

                            if ( !OpenFolderSysCommand.empty( ) )
                            {
                                spdlog::info( "Executing command: {}({})", OpenFolderSysCommand, system( OpenFolderSysCommand.c_str( ) ) );
                            } else
                            {
                                spdlog::warn( "Unknown platform, not opening folder" );
                            }
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
                ImGui::Separator( );

                // Not finished, show build messages
                if ( m_BuildStage != BuildStage::Finished )
                {
                    std::unique_lock Lock( m_BuildThreadStrBufferMutex );
                    m_BuildThreadStrBuffer->Draw( "Build Logs", true, m_Height / 2.F );
                }

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

void
EditorWindow::RenderMenuBar( )
{
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
                m_ShouldActivateOverlay = true;
            }

            ImGui::EndMenu( );
        }

        ImGui::EndMainMenuBar( );
    }
}
