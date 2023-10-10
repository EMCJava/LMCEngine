//
// Created by samsa on 10/9/2023.
//

#include "NetServer.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SYS_ASSERT( x ) REQUIRED_IF( ( x ) != -1, spdlog::trace( "errno: ({})", strerror( errno ) ) )

bool
NetServer::Setup( NetType Type, const std::string& IP, int Port )
{
    struct sockaddr_in my_addr;
    int                on = 1;

    // create a socket
    m_ServerSocketHandle = socket( AF_INET, SOCK_STREAM, 0 );
    SYS_ASSERT( m_ServerSocketHandle )
    {
        // for "Address already in use" error message
        SYS_ASSERT( setsockopt( m_ServerSocketHandle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( int ) ) )
        {
            // server address
            my_addr.sin_family = AF_INET;
            inet_aton( IP.c_str( ), &my_addr.sin_addr );
            my_addr.sin_port = htons( Port );

            SYS_ASSERT( bind( m_ServerSocketHandle, (struct sockaddr*) &my_addr, sizeof( my_addr ) ) )
            {
                spdlog::trace( "Server start at: {}:{}", inet_ntoa( my_addr.sin_addr ), Port );
                m_NetType = Type;
                return true;
            }
        }

        close( m_ServerSocketHandle );
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

int
NetServer::Accept( )
{
    REQUIRED_IF( m_NetType == NetType::TCP && m_ServerSocketHandle != -1 )
    {
        struct sockaddr_in client_addr;

        socklen_t addrlen = sizeof( client_addr );

        spdlog::trace( "Waiting for connection..." );
        auto NewFD = accept( m_ServerSocketHandle, (struct sockaddr*) &client_addr, &addrlen );

        spdlog::trace( "Connected by {}:{}", inet_ntoa( client_addr.sin_addr ), ntohs( client_addr.sin_port ) );

        return NewFD;
    }

    return -1;
}

bool
NetServer::Reveive( int SocketHandle, std::vector<char>& Data )
{
    REQUIRED_IF( SocketHandle != -1 )
    {
        Data.resize( Data.capacity( ) );
        ssize_t nbytes = ::recv( SocketHandle, Data.data( ), Data.size( ), 0 );
        if ( nbytes <= 0 )
        {
            close( SocketHandle );
            SocketHandle = -1;

            Data.clear( );
            spdlog::trace( "Connection closed ({}).", strerror( errno ) );
            return false;
        }

        Data.resize( nbytes );

        return true;
    }

    return false;
}


NetServer::~NetServer( )
{
    if ( m_ServerSocketHandle != -1 )
    {
        close( m_ServerSocketHandle );
        m_ServerSocketHandle = -1;
    }
}
