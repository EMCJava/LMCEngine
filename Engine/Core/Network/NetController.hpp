//
// Created by samsa on 10/9/2023.
//

#pragma once

enum class NetType {
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
    Setup( NetType Type ) = 0;

    virtual bool
    Receive( );

    virtual bool
    Send( );
};
