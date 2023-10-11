//
// Created by Lo Yu Sum on 22/7/2023.
//

#include <Engine/Core/Core.hpp>

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>

#ifdef LMC_WIN
#    include <windows.h>
#    define SHARING_VIOLATION 32L
#elif defined( LMC_UNIX )
#    include <dlfcn.h>
#    define SHARING_VIOLATION EBUSY
#endif

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

#include <Engine/Core/Utilities/PlatformSysUtilities.hpp>

void
MakeSureOperation( auto&& Operation )
{
    while ( !Operation( ) )
    { }
}

DynamicLibrary::DynamicLibrary( bool MakeCopyOnLoad )
{
    m_MakeCopyOnLoad = MakeCopyOnLoad;
}

DynamicLibrary::~DynamicLibrary( )
{
    Unload( );

    if ( !m_DLLTmpPath.empty( ) )
    {
        fs::remove( m_DLLTmpPath );
    }
}

bool
DynamicLibrary::Load( const std::string_view& Path )
{
    if ( !fs::exists( Path ) )
    {
        spdlog::error( "DynamicLibrary::Load: File {} does not exist", Path );
        return false;
    }

    m_DLLLoadPath = m_DLLPath = Path;

    if ( m_MakeCopyOnLoad )
    {
        MakeDLLCopy( );
    }

#ifdef LMC_WIN
    m_DLLHandle = LoadLibrary( m_DLLLoadPath.c_str( ) );
#elif defined( LMC_UNIX )
    m_DLLHandle = dlopen( m_DLLLoadPath.c_str( ), RTLD_LAZY );
#endif

    if ( m_DLLHandle == nullptr )
    {
        PrintSysLinkError( "DynamicLibrary::Load: Can't open and load dynamic library" );
        return false;
    }

    m_DLLModifiedTime = fs::last_write_time( m_DLLPath );

    LoadSymbolAs( "SetEngineContext", m_SetEngineContext );
    REQUIRED( m_SetEngineContext != nullptr );

    return true;
}

void
DynamicLibrary::Unload( )
{
    if ( m_DLLHandle != nullptr )
    {
#ifdef LMC_WIN
        if ( !FreeLibrary( static_cast<HMODULE>( m_DLLHandle ) ) )
#elif defined( LMC_UNIX )
        if ( dlclose( m_DLLHandle ) )
#endif
        {
            PrintSysLinkError( "Failed to free dynamic library" );
        }

        m_DLLHandle        = nullptr;
        m_SetEngineContext = nullptr;
    }
}

bool
DynamicLibrary::Reload( )
{
    Unload( );
    return Load( m_DLLPath );
}

void*
DynamicLibrary::LoadSymbol( const std::string& Name )
{
#ifdef LMC_WIN
    void* result = (void*) GetProcAddress( static_cast<HMODULE>( m_DLLHandle ), Name.c_str( ) );
#elif defined( LMC_UNIX )
    void* result = (void*) dlsym( m_DLLHandle, Name.c_str( ) );
#endif
    if ( result == nullptr )
    {
        PrintSysLinkError( "DynamicLibrary::Load: Can't load function" );
    }

    return result;
}

void
DynamicLibrary::MakeDLLCopy( )
{
    RemoveDLLCopy( );

    fs::path   OriginalDLLPath        = m_DLLPath;
    const auto OriginalDLLParentPath  = OriginalDLLPath.parent_path( );
    const auto HotReloadDLLParentPath = OriginalDLLParentPath / HOT_RELOAD_TMP_PATH;

    if ( !fs::exists( HotReloadDLLParentPath ) ) fs::create_directories( HotReloadDLLParentPath );
    const auto HotReloadDLLPath = HotReloadDLLParentPath / ( OriginalDLLPath.filename( ) );

    m_DLLLoadPath = m_DLLTmpPath = HotReloadDLLPath.string( );

    MakeSureOperation( [ this, retry_left = 10 ]( ) mutable {
        std::error_code OperationError;

        const auto FileSize = fs::file_size( m_DLLPath, OperationError );
        if ( FileSize == 0 )
        {
            // Success, but not overwriting
            //            if ( !OperationError.value( ) )
            //            {
            //                return true;
            //            }

            if ( retry_left >= 1 )
            {
                spdlog::error( "DynamicLibrary::MakeDLLCopy: File size 0, error code {}, retry remaining #{}", OperationError.value( ), retry_left );
                std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
                retry_left--;
                return false;
            }

            // Gave up
        }

        spdlog::info( "DynamicLibrary::MakeDLLCopy: Copying {}bytes.", FileSize );

        return true;
    } );

    MakeSureOperation( [ this, retry_left = 10 ]( ) mutable {
        std::error_code OperationError;
        if ( !fs::copy_file( m_DLLPath, m_DLLTmpPath, fs::copy_options::overwrite_existing, OperationError ) )
        {
            // Success, but not overwriting
            if ( !OperationError.value( ) )
            {
                return true;
            }

            spdlog::error( "DynamicLibrary::MakeDLLCopy: Not copying file, {}({})", OperationError.message( ), OperationError.value( ) );
            if ( retry_left >= 1 && SHARING_VIOLATION == OperationError.value( ) )
            {
                spdlog::error( "DynamicLibrary::MakeDLLCopy: File accessing by another process, retry remaining #{}", retry_left );
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
                retry_left--;
                return false;
            }

            // Gave up
        }

        return true;
    } );

    const auto& DLLFilename             = OriginalDLLPath.stem( );
    const auto& DLLFileWithoutExtension = OriginalDLLParentPath / DLLFilename;

    const fs::path DLLFileDebugInfo          = DLLFileWithoutExtension.string( ) + ".pdb";
    const fs::path DLLFileHotReloadDebugInfo = HotReloadDLLParentPath / DLLFileDebugInfo.filename( );

    if ( fs::exists( DLLFileDebugInfo ) )
    {
        if ( fs::exists( DLLFileHotReloadDebugInfo ) )
        {
            spdlog::info( "DynamicLibrary::MakeDLLCopy: Removing debug info for hot-reloading: {}", DLLFileDebugInfo.string( ) );
            MakeSureOperation( [ &DLLFileHotReloadDebugInfo, retry_left = 10 ]( ) mutable {
                std::error_code OperationError;
                if ( !fs::remove( DLLFileHotReloadDebugInfo, OperationError ) )
                {
                    spdlog::error( "DynamicLibrary::MakeDLLCopy: Failed to remove debug info, further hot reloading may fail, {}({})", OperationError.message( ), OperationError.value( ) );
                    if ( retry_left >= 1 && SHARING_VIOLATION == OperationError.value( ) )
                    {
                        spdlog::error( "DynamicLibrary::MakeDLLCopy: File accessing by another process, retry remaining #{}", retry_left );
                        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
                        retry_left--;
                        return false;
                    }

                    // Gave up
                }

                return true;
            } );
        }

        spdlog::info( "DynamicLibrary::MakeDLLCopy: Copying debug info for hot-reloading: {}", DLLFileDebugInfo.string( ) );
        MakeSureOperation( [ this, &DLLFileDebugInfo, &DLLFileHotReloadDebugInfo, retry_left = 10 ]( ) mutable {
            std::error_code OperationError;
            if ( !fs::copy_file( DLLFileDebugInfo, DLLFileHotReloadDebugInfo, fs::copy_options::overwrite_existing, OperationError ) )
            {
                // Success, but not overwriting
                if ( !OperationError.value( ) )
                {
                    return true;
                }

                spdlog::error( "DynamicLibrary::MakeDLLCopy: Not copying file, {}({})", OperationError.message( ), OperationError.value( ) );
                if ( retry_left >= 1 && SHARING_VIOLATION == OperationError.value( ) )
                {
                    spdlog::error( "DynamicLibrary::MakeDLLCopy: File accessing by another process, retry remaining #{}", retry_left );
                    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
                    retry_left--;
                    return false;
                }

                // Gave up
            }

            return true;
        } );
    }
}

bool
DynamicLibrary::ShouldReload( ) const
{
    try
    {
        const auto& NewDLLModifiedTime = fs::last_write_time( m_DLLPath );
        return NewDLLModifiedTime != m_DLLModifiedTime;
    }
    catch ( const fs::filesystem_error& e )
    {
        spdlog::error( "DynamicLibrary::ShouldReload: {}", e.what( ) );
        return false;
    }
}

void
DynamicLibrary::RemoveDLLCopy( )
{
    /*
     *
     * Existing copy, remove
     *
     * */
    if ( !m_DLLTmpPath.empty( ) )
    {
        fs::remove( m_DLLTmpPath );

        if ( !m_DLLDependencies.empty( ) )
        {
            std::for_each( m_DLLDependencies.begin( ), m_DLLDependencies.end( ), []( const auto& path ) {
                fs::remove( path );
            } );

            m_DLLDependencies.clear( );
        }
    }
}

void
DynamicLibrary::SetEngineContext( class Engine* ptr )
{
    m_SetEngineContext( ptr );
}
