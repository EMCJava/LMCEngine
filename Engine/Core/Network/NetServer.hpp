//
// Created by samsa on 10/9/2023.
//

#pragma once

#include "NetController.hpp"

class NetServer : public NetController
{
public:
    ~NetServer( );

    using NetController::ReceiveFrom;
    using NetController::SendTo;

    bool
    Setup( NetType Type, const std::string& IP, int Port, bool Bind = true );

    bool
    Listen( int ClientCount );

    std::shared_ptr<class NetClient>
    Accept( );

protected:
    bool
    ReceiveFrom( std::vector<char>& Data, struct sockaddr_in& Address ) override;

    bool
    SendTo( const std::vector<char>& Data, const struct sockaddr_in& Target ) override;

    SocketTy m_ServerSocketHandle = -1;
};
