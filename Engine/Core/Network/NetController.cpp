//
// Created by samsa on 10/9/2023.
//

#include "NetController.hpp"
#include "NetPlatform.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

void
NetController::CloseSocket( SocketTy Sock )
{
#ifdef LMC_WIN
    closesocket( Sock );
#elif defined( LMC_UNIX )
    close( Sock );
#endif
}

void
NetController::NetGlobalInitialize( )
{
#ifdef LMC_WIN
    WSADATA WsData;
    REQUIRED( WSAStartup( MAKEWORD( 2, 2 ), &WsData ) == 0 )
#endif
}

void
NetController::NetGlobalShutdown( )
{
#ifdef LMC_WIN
    WSACleanup( );
#endif
}

bool
NetController::ReceiveFrom( std::vector<char>& Data, NetPoint& Address )
{
    struct sockaddr_in addr;

    bool result = ReceiveFrom( Data, addr );
    if ( result )
    {
        Address.IP   = inet_ntoa( addr.sin_addr );
        Address.Port = ntohs( addr.sin_port );
    }

    return result;
}

bool
NetController::SendTo( const std::vector<char>& Data, const NetController::NetPoint& Target )
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port   = htons( Target.Port );
    inet_pton( AF_INET, Target.IP.c_str( ), &addr.sin_addr );

    return SendTo( Data, addr );
}
