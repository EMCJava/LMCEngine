//
// Created by samsa on 10/9/2023.
//

#pragma once

#include "NetController.hpp"

class NetClient : public NetController
{
private:
    explicit NetClient( int WrapSocket );

public:
    NetClient( ) = default;

    ~NetClient( );

    bool
    Setup( NetType Type, const std::string& IP, int Port ) override;

    bool
    Receive( std::vector<char>& Data ) override;

    bool
    Send( std::vector<char>& Data ) override;

    static std::shared_ptr<NetClient>
    WrapSocket( int Sock );

protected:
    int m_SocketHandle = -1;
};
