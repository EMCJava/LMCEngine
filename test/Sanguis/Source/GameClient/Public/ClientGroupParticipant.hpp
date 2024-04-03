//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include "Message.hpp"

#include <deque>

namespace SanguisNet
{
class ClientGroupParticipant
{
public:
    using PacketCallbackSig  = std::function<void( SanguisNet::Message& )>;
    using ConncetCallbackSig = std::function<void( )>;

    ClientGroupParticipant( asio::io_context&                            io_context,
                            const asio::ip::tcp::resolver::results_type& EndPoint );

    void Post( const SanguisNet::Message& msg );

    void Close( );

    void SetPacketCallback( const PacketCallbackSig& Callback ) { m_PackageCallback = Callback; }
    void SetConnectCallback( const ConncetCallbackSig& Callback ) { m_ConnectCallback = Callback; }

private:
    void DoConnect( const asio::ip::tcp::resolver::results_type& EndPoint );

    void ReadMessageHeader( );

    void ReadMessageBody( );

    void SendTopMessage( );

    void Terminate( );

private:
    asio::io_context&                     m_ControlContext;
    asio::ip::tcp::socket                 m_Socket;
    asio::ip::tcp::resolver::results_type m_DesiredEndPoint;
    asio::ip::tcp::endpoint               m_RemoteEndpoint;
    SanguisNet::Message                   m_ReadMsg;
    std::deque<SanguisNet::Message>       m_MessageQueue;

    PacketCallbackSig  m_PackageCallback;
    ConncetCallbackSig m_ConnectCallback;

    bool m_ConnectionReady = false;
};
}   // namespace SanguisNet