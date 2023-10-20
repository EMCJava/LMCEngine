//
// Created by loyus on 7/2/2023.
//

#include "Project.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( ProjectConfig, project_name, project_build_target, root_concept )

namespace
{
nlohmann::json
ReadJsonFile( const std::filesystem::path& Path )
{
    if ( !std::filesystem::exists( Path ) )
    {
        spdlog::error( "Attempting to read json file that does not exist: {}", Path.string( ) );
        return { };
    }

    std::ifstream ifs( Path );
    return nlohmann::json::parse( ifs );
}
}   // namespace

auto
Project::GetConfig( ) -> ProjectConfig&
{
    return m_Config;
}

void
Project::LoadProject( const std::string& ProjectFilePath )
{
    m_ProjectFilePath = "";
    spdlog::info( "Reset project, Open project: [{}]", ProjectFilePath );

    if ( !ProjectFilePath.ends_with( ".lmce" ) )
    {
        spdlog::warn( "Unknown file extension: {}", ProjectFilePath );
        return;
    }

    m_Config = ReadJsonFile( ProjectFilePath );

    spdlog::info( "Project name: {}", m_Config.project_name );
    std::filesystem::path ProjectPath( ProjectFilePath );
    ProjectPath = ProjectPath.parent_path( );

    // Setup project path config
#ifdef LMC_EDITOR
    {
        auto ProjectPathConfigFile = ProjectPath / "ProjectCache/ProjectPath.config";
        auto ProjectPathConfig     = ReadJsonFile( ProjectPathConfigFile );

        m_Config.shared_library_path_format = ProjectPathConfig[ "shared_library_path_" CMAKE_BUILD_TYPE "_format" ];
    }
#else
    m_Config.shared_library_path_format = "";
#endif

    if ( m_Config.editor_layout_path.empty( ) )
    {
        const auto EditorLayoutPath = ProjectPath / "Editor/layout.ini";
        if ( std::filesystem::exists( EditorLayoutPath ) )
        {
            m_Config.editor_layout_path = EditorLayoutPath.string( );
        }
    }

    m_ProjectFilePath = ProjectFilePath;

    // Make relative path works in user project
    std::filesystem::current_path( ProjectPath );
}

void
Project::SaveProject( )
{
    if ( m_ProjectFilePath.empty( ) )
    {
        spdlog::warn( "Project path empty, unable to save project" );
        return;
    }

    std::ofstream ofs( m_ProjectFilePath );

    if ( !ofs )
    {
        spdlog::error( "Unable to open project path: [{}]", m_ProjectFilePath );
        return;
    }

    ofs << nlohmann::json { m_Config }[ 0 ].dump( 4 );
}

Project::~Project( )
{
    spdlog::info( "Close project: {}", m_Config.project_name );
}

const std::string&
Project::GetProjectFilePath( ) const
{
    return m_ProjectFilePath;
}
