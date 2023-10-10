//
// Created by samsa on 10/9/2023.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

enum class NetType {
    None,
    TCP,
    UDP
};

class NetController
{
public:
    NetController( )          = default;
    virtual ~NetController( ) = default;

    NetController( const NetController& )            = delete;
    NetController& operator=( const NetController& ) = delete;
    NetController( NetController&& )                 = delete;
    NetController& operator=( NetController&& )      = delete;

    virtual bool
    Setup( NetType Type, const std::string& IP, int Port ) = 0;

    virtual bool
    Receive( std::vector<char>& Data ) { }

    virtual bool
    Send( std::vector<char>& Data ) { }

protected:
    NetType m_NetType;
};
