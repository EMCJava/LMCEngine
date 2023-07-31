//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <imgui.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/File/OSFile.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Engine.hpp>

#include <filesystem>

void
EditorWindow::Update( )
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

        ImGui::EndMainMenuBar( );
    }

    {
        ImGui::Begin( "Hierarchy" );

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

        const auto WindowDimensions = ImGui::GetContentRegionAvail( );
        m_HotReloadWindowWidth      = WindowDimensions.x;
        m_HotReloadWindowHeight     = WindowDimensions.y;

        ImGui::Image( reinterpret_cast<void*>( m_PreviousFrameTextureID ),
                      ImVec2( m_HotReloadWindowWidth, m_HotReloadWindowHeight ),
                      ImVec2( 0, 1 ),
                      ImVec2( 1, 0 ) );

        ImGui::EndChild( );

        ImGui::End( );
    }
}

std::pair<float, float>
EditorWindow::GetHowReloadWindowDimensions( ) const
{
    return { m_HotReloadWindowWidth, m_HotReloadWindowHeight };
}

void
EditorWindow::SetPreviousFrameTexture( uint32_t TextureID )
{
    m_PreviousFrameTextureID = TextureID;
}
