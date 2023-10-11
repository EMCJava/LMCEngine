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
NetServer::Setup( NetType Type, const std::string& IP, int Port, bool Bind )
{
    if ( m_ServerSocketHandle != -1 )
    {
        spdlog::trace( "Recreating socket ({})", m_ServerSocketHandle );

        NetController::CloseSocket( m_ServerSocketHandle );
        m_ServerSocketHandle = -1;
    }

    int TypeCode = 0;
    switch ( Type )
    {
    case NetType::TCP:
        TypeCode = SOCK_STREAM;
        break;
    case NetType::UDP:
        TypeCode = SOCK_DGRAM;
        break;
    default:
        spdlog::error( "Invalid network type" );
        return false;
    }

    // create a socket
    m_ServerSocketHandle = socket( AF_INET, TypeCode, 0 );
    SYS_ASSERT( m_ServerSocketHandle )
    {
        if ( Bind )
        {
            int on = 1;
#ifdef LMC_UNIX
            // for "Address already in use" error message
            SYS_ASSERT( setsockopt( m_ServerSocketHandle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( int ) ) )
#endif
            {
                // server address
                struct sockaddr_in my_addr;
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
        } else
        {
            spdlog::trace( "Socket setup at: {}:{} (non-bind)", IP, Port );
            m_NetType = Type;
            return true;
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

bool
NetServer::ReceiveFrom( std::vector<char>& Data, struct sockaddr_in& Address )
{
    REQUIRED_IF( m_NetType == NetType::UDP && m_ServerSocketHandle != -1 )
    {
        Data.resize( Data.capacity( ) );
        int AddressLength = sizeof( Address );
        int nbytes = ::recvfrom( m_ServerSocketHandle, Data.data( ), Data.size( ), 0, (struct sockaddr*) &Address, &AddressLength );
        if ( nbytes <= 0 )
        {
            PrintSysNetError( "recvfrom: " );
            Data.clear( );
            return false;
        }

        REQUIRED( AddressLength == sizeof( Address ) )
        Data.resize( nbytes );

        return true;
    }

    return false;
}

bool
NetServer::SendTo( const std::vector<char>& Data, const struct sockaddr_in& Target )
{
    REQUIRED_IF( m_NetType == NetType::UDP && m_ServerSocketHandle != -1 )
    {
        size_t DataSent = 0;
        while ( DataSent != Data.size( ) )
        {
            int nbytes = ::sendto( m_ServerSocketHandle, Data.data( ) + DataSent, Data.size( ) - DataSent, 0, (struct sockaddr*) &Target, sizeof( Target ) );
            if ( nbytes <= 0 )
            {
                PrintSysNetError( "sendto: " );
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
