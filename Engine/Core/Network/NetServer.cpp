//
// Created by samsa on 10/9/2023.
//

#include "NetServer.hpp"
#include "NetPlatform.hpp"

#include <Engine/Core/Utilities/PlatformSysUtilities.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Network/NetClient.hpp>

#define SYS_ASSERT( x ) REQUIRED_IF( ( x ) != -1, PrintSysNetError( "" ) )

bool
NetServer::Setup( NetType Type, const std::string& IP, int Port )
{
    struct sockaddr_in my_addr;
    int                on = 1;

    if ( m_ServerSocketHandle != -1 )
    {
        spdlog::trace( "Recreating socket ({})", m_ServerSocketHandle );

        NetController::CloseSocket( m_ServerSocketHandle );
        m_ServerSocketHandle = -1;
    }

    // create a socket
    m_ServerSocketHandle = socket( AF_INET, SOCK_STREAM, 0 );
    SYS_ASSERT( m_ServerSocketHandle )
    {
#ifdef LMC_UNIX
        // for "Address already in use" error message
        SYS_ASSERT( setsockopt( m_ServerSocketHandle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( int ) ) )
#endif
        {
            // server address
            my_addr.sin_family = AF_INET;
            inet_pton( AF_INET, IP.c_str( ), &my_addr.sin_addr );
            my_addr.sin_port = htons( Port );

            SYS_ASSERT( bind( m_ServerSocketHandle, (struct sockaddr*) &my_addr, sizeof( my_addr ) ) )
            {
                spdlog::trace( "Server start at: {}:{}", inet_ntoa( my_addr.sin_addr ), Port );
                m_NetType = Type;
                return true;
            }
        }

        NetController::CloseSocket( m_ServerSocketHandle );
        m_ServerSocketHandle = -1;
    }

    return false;
}

bool
NetServer::Listen( int ClientCount )
{
    REQUIRED_IF( m_NetType == NetType::TCP && m_ServerSocketHandle )
    {
        SYS_ASSERT( listen( m_ServerSocketHandle, ClientCount ) )
        {
            spdlog::trace( "Ready for connection..." );
            return true;
        }
    }

    return false;
}

std::shared_ptr<NetClient>
NetServer::Accept( )
{
    REQUIRED_IF( m_NetType == NetType::TCP && m_ServerSocketHandle != -1 )
    {
        struct sockaddr_in client_addr;

        socklen_t addrlen = sizeof( client_addr );

        spdlog::trace( "Waiting for connection..." );
        auto NewFD = accept( m_ServerSocketHandle, (struct sockaddr*) &client_addr, &addrlen );

        spdlog::trace( "Connected by {}:{}", inet_ntoa( client_addr.sin_addr ), ntohs( client_addr.sin_port ) );

        return NetClient::WrapSocket( NewFD );
    }

    return nullptr;
}

NetServer::~NetServer( )
{
    if ( m_ServerSocketHandle != -1 )
    {
        NetController::CloseSocket( m_ServerSocketHandle );
        m_ServerSocketHandle = -1;
    }
}
