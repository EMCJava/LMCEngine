//
// Created by samsa on 10/9/2023.
//

#pragma once

#include "NetController.hpp"

class NetServer : public NetController
{
public:
    ~NetServer( );

    bool
    Setup( NetType Type, const std::string& IP, int Port ) override;

    bool
    Listen( int ClientCount );

    std::shared_ptr<class NetClient>
    Accept( );

private:
    SocketTy m_ServerSocketHandle = -1;
};
