//
// Created by samsa on 10/9/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <string>
#include <vector>
#include <memory>

#ifdef LMC_UNIX
using SocketTy = int;
#elif defined( LMC_WIN )
// Will later static_assert for type checking
using SocketTy = uint64_t;
#endif

enum class NetType {
    None,
    TCP,
    UDP
};

class NetController
{
public:
    struct NetPoint {
        std::string IP;
        int         Port;
    };

    NetController( )          = default;
    virtual ~NetController( ) = default;

    NetController( const NetController& )            = delete;
    NetController& operator=( const NetController& ) = delete;
    NetController( NetController&& )                 = delete;
    NetController& operator=( NetController&& )      = delete;

    virtual bool
    Receive( std::vector<char>& Data ) { return false; }

    virtual bool
    Send( const std::vector<char>& Data ) { return false; }

    virtual bool
    ReceiveFrom( std::vector<char>& Data, NetPoint& Address );

    virtual bool
    SendTo( const std::vector<char>& Data, const NetPoint& Target );

    static void
    CloseSocket( SocketTy Sock );

    static void
    NetGlobalInitialize( );

    static void
    NetGlobalShutdown( );

protected:
    virtual bool
    ReceiveFrom( std::vector<char>& Data, struct sockaddr_in& Address ) { return false; }

    virtual bool
    SendTo( const std::vector<char>& Data, const struct sockaddr_in& Target ) { return false; }

    NetType m_NetType;
};