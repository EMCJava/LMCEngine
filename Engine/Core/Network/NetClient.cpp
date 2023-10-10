//
// Created by samsa on 10/9/2023.
//

#include "NetClient.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SYS_ASSERT( x ) REQUIRED_IF( ( x ) != -1, spdlog::trace( "errno: ({})", strerror( errno ) ) )

NetClient::~NetClient( )
{
    if ( m_SocketHandle != -1 )
    {
        close( m_SocketHandle );
        m_SocketHandle = -1;
    }
}

bool
NetClient::Setup( NetType Type, const std::string& IP, int Port )
{
    struct sockaddr_in serv_name;

    if ( m_SocketHandle != -1 )
    {
        spdlog::trace( "Recreating socket ({})", m_SocketHandle );

        close( m_SocketHandle );
        m_SocketHandle = -1;
    }

    // create a socket
    m_SocketHandle = socket( AF_INET, SOCK_STREAM, 0 );
    SYS_ASSERT( m_SocketHandle )
    {
        // server address
        serv_name.sin_family = AF_INET;
        inet_aton( IP.c_str( ), &serv_name.sin_addr );
        serv_name.sin_port = htons( Port );

        SYS_ASSERT( connect( m_SocketHandle, (struct sockaddr*) &serv_name, sizeof( serv_name ) ) )
        {
            spdlog::trace( "Connected at: {}:{}", inet_ntoa( serv_name.sin_addr ), Port );
            m_NetType = Type;
            return true;
        }

        close( m_SocketHandle );
        m_SocketHandle = -1;
    }

    return false;
}

bool
NetClient::Receive( std::vector<char>& Data )
{
    REQUIRED_IF( m_SocketHandle != -1 )
    {
        Data.resize( Data.capacity( ) );
        ssize_t nbytes = ::recv( m_SocketHandle, Data.data( ), Data.size( ), 0 );
        if ( nbytes <= 0 )
        {
            close( m_SocketHandle );
            m_SocketHandle = -1;

            Data.clear( );
            spdlog::trace( "Connection closed ({}).", strerror( errno ) );
            return false;
        }

        Data.resize( nbytes );

        return true;
    }

    return false;
}

bool
NetClient::Send( std::vector<char>& Data )
{
    REQUIRED_IF( m_SocketHandle != -1 )
    {
        size_t DataSent = 0;
        while ( DataSent != Data.size( ) )
        {
            ssize_t nbytes = ::send( m_SocketHandle, Data.data( ) + DataSent, Data.size( ) - DataSent, 0 );
            if ( nbytes <= 0 )
            {
                close( m_SocketHandle );
                m_SocketHandle = -1;

                Data.clear( );
                spdlog::trace( "Connection closed ({}).", strerror( errno ) );
                return false;
            } else
            {
                DataSent += nbytes;
            }
        }

        return true;
    }

    return false;
}

std::shared_ptr<NetClient>
NetClient::WrapSocket( int Sock )
{
    return std::shared_ptr<NetClient>( new NetClient( Sock ) );
}

NetClient::NetClient( int WrapSocket )
    : m_SocketHandle( WrapSocket )
{
}
