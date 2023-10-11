//
// Created by samsa on 10/11/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#ifdef LMC_UNIX

#    include <sys/socket.h>
#    include <netinet/in.h>
#    include <arpa/inet.h>
#    include <unistd.h>
#    include <stdio.h>
#    include <stdlib.h>

#elif defined( LMC_WIN )

// 'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
#    define _WINSOCK_DEPRECATED_NO_WARNINGS

#    define _CRT_SECURE_NO_WARNINGS

#    include <WinSock2.h>
#    include <ws2tcpip.h>
#    pragma comment( lib, "ws2_32.lib" )

static_assert( std::is_same_v<SOCKET, uint64_t> );
static_assert( INVALID_SOCKET == SOCKET_ERROR );
static_assert( INVALID_SOCKET == -1 );
#endif
