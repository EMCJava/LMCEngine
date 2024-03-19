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
    ClientGroupParticipant( asio::io_context&                            io_context,
                            const asio::ip::tcp::resolver::results_type& EndPoint );

    void Post( const SanguisNet::Message& msg );

    void Close( );

private:
    void DoConnect( const asio::ip::tcp::resolver::results_type& EndPoint );

    void ReadMessageHeader( );

    void ReadMessageBody( );

    void SendTopMessage( );

    void Terminate( );

private:
    asio::io_context&               m_ControlContext;
    asio::ip::tcp::socket           m_Socket;
    SanguisNet::Message             m_ReadMsg;
    std::deque<SanguisNet::Message> m_MessageQueue;

    bool m_ConnectionReady = false;
};
}   // namespace SanguisNet