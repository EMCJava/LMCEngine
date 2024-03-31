//
// Created by EMCJava on 3/20/2024.
//

#pragma once

#include "Message.hpp"
#include "ServerSectionGroup.hpp"

#include <memory>

#include <spdlog/spdlog.h>

namespace SanguisNet
{
template <typename ParticipantTy, typename Ty>
inline asio::awaitable<void>
ServerSectionAcceptor( asio::ip::tcp::acceptor            acceptor,
                       const std::shared_ptr<Ty>&         Section,
                       const asio::steady_timer::duration timeout = std::chrono::seconds( 10 ) )
{
    using namespace std::chrono_literals;

    asio::steady_timer DeadlineTimer( acceptor.get_executor( ) );

    std::function<void( std::error_code )> TimeoutHandler;
    TimeoutHandler = [ & ]( std::error_code ec ) {
        const auto LocalEndpoint = acceptor.local_endpoint( );
        if ( ec ) return;
        if ( Section->GetParticipantsCount( ) == 0 )
        {
            spdlog::info( "{}:{} party releasing due to inactivity ...", LocalEndpoint.address( ).to_string( ), LocalEndpoint.port( ) );

            // Release section
            acceptor.close( );
        } else
        {
            spdlog::info( "{}:{} waiting for a client to connect...", LocalEndpoint.address( ).to_string( ), LocalEndpoint.port( ) );
            DeadlineTimer.expires_after( timeout );
            DeadlineTimer.async_wait( TimeoutHandler );
        }
    };

    while ( true )
    {
        DeadlineTimer.expires_after( timeout );
        DeadlineTimer.async_wait( TimeoutHandler );
        auto Socket = co_await acceptor.async_accept( asio::use_awaitable );
        DeadlineTimer.cancel( );

        const asio::ip::tcp::endpoint RemoteEndpoint = Socket.remote_endpoint( );

        const auto LocalEndpoint = Socket.local_endpoint( );
        spdlog::info( "Client IP: {}:{} connected to {}:{}", RemoteEndpoint.address( ).to_string( ), RemoteEndpoint.port( ), LocalEndpoint.address( ).to_string( ), LocalEndpoint.port( ) );

        // Using shared_from_this, it won't be deleted
        std::make_shared<ParticipantTy>(
            std::move( Socket ),
            Section )
            ->StartListening( );
    }
}
}   // namespace SanguisNet