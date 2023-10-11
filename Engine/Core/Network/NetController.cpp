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
