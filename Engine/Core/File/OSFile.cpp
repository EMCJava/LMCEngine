//
// Created by loyus on 7/2/2023.
//

#include "OSFile.hpp"

#include <stdexcept>

#include <spdlog/spdlog.h>

std::string
OSFile::PickFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path )
{
    nfdchar_t*  outPath = nullptr;
    nfdresult_t result  = NFD::OpenDialog( outPath, filter.data( ), filter.size( ), default_path );

    if ( result == NFD_OKAY )
    {
        std::string path( outPath );
        NFD::FreePath( outPath );

        return path;
    } else if ( result == NFD_CANCEL )
    {
        return "";
    } else
    {
        throw std::runtime_error( std::string( "OSFile::PickFile error: " ) + NFD_GetError( ) );
    }
}

std::string
OSFile::SaveFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path )
{
    nfdchar_t*  outPath = nullptr;
    nfdresult_t result  = NFD::OpenDialog( outPath, filter.data( ), filter.size( ), default_path );

    if ( result == NFD_OKAY )
    {
        std::string path( outPath );
        NFD::FreePath( outPath );

        return path;
    } else if ( result == NFD_CANCEL )
    {
        return "";
    } else
    {
        throw std::runtime_error( std::string( "OSFile::PickFile error: " ) + NFD_GetError( ) );
    }
}

std::string
OSFile::PickFolder( const char* default_path )
{
    nfdchar_t*  outPath = nullptr;
    nfdresult_t result  = NFD::PickFolder( outPath, default_path );

    if ( result == NFD_OKAY )
    {
        std::string path( outPath );
        NFD::FreePath( outPath );

        return path;
    } else if ( result == NFD_CANCEL )
    {
        return "";
    } else
    {
        throw std::runtime_error( std::string( "OSFile::PickFolder error: " ) + NFD_GetError( ) );
    }
}

bool
OSFile::CopyFolder( const std::filesystem::path& Path, std::filesystem::path Destination )
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
