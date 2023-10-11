//
// Created by samsa on 10/11/2023.
//

#pragma once

#include <string_view>

#ifdef LMC_WIN
#    include <windows.h>
#elif defined( LMC_UNIX )
#    include <dlfcn.h>
#    include <errno.h>
#    include <string.h>
#endif

#include <spdlog/spdlog.h>

namespace
{
inline void
PrintWinError( const std::string_view& message, auto&& ErrorFunc )
{
#ifdef LMC_WIN
    auto errorMessageID = ErrorFunc( );

    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  nullptr, errorMessageID, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR) &messageBuffer, 0, nullptr );

    spdlog::error( "{}: {}", message, std::string_view( messageBuffer, size - 2 ) );

    // Free the Win32's string's buffer.
    LocalFree( messageBuffer );
#endif
}

inline void
PrintUnixLinkError( const std::string_view& message )
{
#ifdef LMC_UNIX
    std::string DLErrorString = dlerror( );
    spdlog::error( "{}: {}", message, DLErrorString );
#endif
}
inline void
PrintUnixError( const std::string_view& message )
{
#ifdef LMC_UNIX
    std::string DLErrorString = strerror( errno );
    spdlog::error( "{}: {}", message, DLErrorString );
#endif
}

}   // namespace

inline void
PrintSysLinkError( std::string_view message )
{
#ifdef LMC_WIN
    PrintWinError( message, ::GetLastError );
#elif defined( LMC_UNIX )
    PrintUnixLinkError( message );
#endif
}

inline void
PrintSysNetError( std::string_view message )
{
#ifdef LMC_WIN
    PrintWinError( message, ::WSAGetLastError );
#elif defined( LMC_UNIX )
    PrintUnixError( message );
#endif
}

inline void
PrintSysError( std::string_view message )
{
#ifdef LMC_WIN
    PrintWinError( message, ::GetLastError );
#elif defined( LMC_UNIX )
    PrintUnixError( message );
#endif
}