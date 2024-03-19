//
// Created by EMCJava on 3/20/2024.
//

#pragma once

#include "Message.hpp"
#include "ServerSectionGroup.hpp"

#include <memory>
#include <iostream>

namespace SanguisNet
{
template <typename ParticipantTy, typename Ty>
inline asio::awaitable<void>
ServerSectionAcceptor( asio::ip::tcp::acceptor    acceptor,
                       const std::shared_ptr<Ty>& Section )
{
    using namespace std::chrono_literals;

    asio::steady_timer DeadlineTimer( acceptor.get_executor( ) );

    std::function<void( std::error_code )> TimeoutHandler;
    TimeoutHandler = [ & ]( std::error_code ec ) {
        const auto LocalEndpoint = acceptor.local_endpoint( );
        if ( ec ) return;
        if ( Section->GetParticipantsCount( ) == 0 )
        {
            std::cout << "[" << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( ) << "] Party releasing..." << std::endl;

            // Release section
            acceptor.close( );
        } else
        {
            std::cout << "[" << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( ) << "] Waiting for a client to connect..." << std::endl;
            DeadlineTimer.expires_after( 10s );
            DeadlineTimer.async_wait( TimeoutHandler );
        }
    };

    while ( true )
    {
        DeadlineTimer.expires_after( 10s );
        DeadlineTimer.async_wait( TimeoutHandler );
        auto Socket = co_await acceptor.async_accept( asio::use_awaitable );
        DeadlineTimer.cancel( );

        const asio::ip::tcp::endpoint remoteEndpoint = Socket.remote_endpoint( );

        const auto LocalEndpoint = acceptor.local_endpoint( );
        std::cout << "Client IP: "
                  << remoteEndpoint.address( ).to_string( ) << ":" << remoteEndpoint.port( )
                  << " connecting to "
                  << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( )
                  << std::endl;

        // Using shared_from_this, it won't be deleted
        std::make_shared<ParticipantTy>(
            std::move( Socket ),
            Section )
            ->StartListening( );
    }
}
}   // namespace SanguisNet