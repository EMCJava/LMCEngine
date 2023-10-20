//
// Created by loyus on 7/2/2023.
//

#include "OSFile.hpp"

#include <Engine/Core/Core.hpp>

#include <stdexcept>

#include <spdlog/spdlog.h>

namespace
{
#ifdef LMC_LINUX
// NFD will crash on multiple if no all linux platform I tried
inline std::string
ZenityFilePicker( const std::string& AddtionalArgs )
{
    FILE* pipe = popen( ( "zenity --file-selection " + AddtionalArgs ).c_str( ), "r" );
    if ( !pipe )
    {
        spdlog::error( "Could not open pipe for zenity(file selection dialog for Linux)" );
        return "";
    }

    char        buffer[ 1024 ];
    std::string result = "";

    while ( !feof( pipe ) )
    {
        if ( fgets( buffer, 1024, pipe ) != nullptr )
        {
            result += buffer;
        }
    }

    pclose( pipe );

    // Remove the trailing newline character, if present
    if ( !result.empty( ) && result[ result.length( ) - 1 ] == '\n' )
    {
        result.pop_back( );
    }

    return result;
}
#endif
}   // namespace

std::string
OSFile::PickFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path )
{
#ifdef LMC_LINUX
    // NFD will crash on multiple if no all linux platform I tried

    // --file-filter='Music files (ogg,wav,aac) | *.ogg *.wav *.aac'

    std::string zenity_filter;
    for ( const auto& FileType : filter )
    {
        zenity_filter += "--file-filter=\"";
        zenity_filter += FileType.name;
        zenity_filter += " | ";
        zenity_filter += std::string( "*." ) + FileType.spec;
        zenity_filter += "\" ";
    }

    if ( default_path != nullptr )
    {
        return ZenityFilePicker( zenity_filter + "--filename=\"" + std::string( default_path ) + "\"" );
    } else
    {
        return ZenityFilePicker( zenity_filter );
    }

#else
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
#endif
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
#ifdef LMC_LINUX
    // NFD will crash on multiple if no all linux platform I tried

    if ( default_path != nullptr )
    {
        return ZenityFilePicker( "--directory --filename=\"" + std::string( default_path ) + "\"" );
    } else
    {
        return ZenityFilePicker( "--directory" );
    }

#else
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
#endif
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
