//
// Created by Lo Yu Sum on 2024/03/19.
//

#include "GroupParticipantTcp.hpp"

#include "ServerSectionGroup.hpp"

#include <spdlog/spdlog.h>

SanguisNet::GroupParticipantTcp::GroupParticipantTcp( asio::ip::tcp::socket Socket, std::shared_ptr<SanguisNet::ServerSectionGroup> BelongsTo )
    : m_Socket( std::move( Socket ) )
    , m_MessageSignal( m_Socket.get_executor( ) )
{
    m_BelongsTo = std::move( BelongsTo );

    // Make sure it will not expire, used as signal
    m_MessageSignal.expires_at( std::chrono::steady_clock::time_point::max( ) );
}

void
SanguisNet::GroupParticipantTcp::StartListening( )
{
    TransferSection( m_BelongsTo );

    // Spawn reader and write job
    asio::co_spawn( m_Socket.get_executor( ), [ self = shared_from_this( ) ] { return std::dynamic_pointer_cast<GroupParticipantTcp>( self )->ReaderJob( ); }, asio::detached );
    asio::co_spawn( m_Socket.get_executor( ), [ self = shared_from_this( ) ] { return std::dynamic_pointer_cast<GroupParticipantTcp>( self )->WriterJob( ); }, asio::detached );
}

void
SanguisNet::GroupParticipantTcp::Deliver( const SanguisNet::Message& Msg )
{
    m_MessageQueue.push_back( Msg );
    // Signal writer to perform a write action
    m_MessageSignal.cancel_one( );
}

asio::awaitable<void>
SanguisNet::GroupParticipantTcp::ReaderJob( )
{
    try
    {
        auto                SharedFromThis = shared_from_this( );
        SanguisNet::Message Msg;
        while ( true )
        {
            // First read header to get the actual message length
            co_await asio::async_read( m_Socket, asio::buffer( &Msg.header, sizeof( Msg.header ) ), asio::use_awaitable );

            if ( Msg.header.length > MessageDataLength )
                throw std::range_error( "Message header corrupted" );

            // Second, read the message body
            co_await asio::async_read( m_Socket, asio::buffer( &Msg.data, Msg.header.length ), asio::use_awaitable );

            // Broadcast to all participant
            m_BelongsTo->HandleMessage( SharedFromThis, Msg );

            // Reset header
            Msg.header = { };
        }
    }
    catch ( std::exception& )
    {
        const auto RemoteEndpoint = m_Socket.remote_endpoint( );
        const auto LocalEndpoint  = m_Socket.local_endpoint( );
        spdlog::info( "Client IP: {}:{} disconnected from {}:{}", RemoteEndpoint.address( ).to_string( ), RemoteEndpoint.port( ), LocalEndpoint.address( ).to_string( ), LocalEndpoint.port( ) );
        Terminate( );
    }
}

asio::awaitable<void>
SanguisNet::GroupParticipantTcp::WriterJob( )
{
    try
    {
        while ( m_Socket.is_open( ) )
        {
            if ( m_MessageQueue.empty( ) )
            {
                asio::error_code ec;
                co_await m_MessageSignal.async_wait( redirect_error( asio::use_awaitable, ec ) );
            } else
            {
                co_await asio::async_write( m_Socket, m_MessageQueue.front( ).GetMinimalAsioBuffer( ), asio::use_awaitable );
                m_MessageQueue.pop_front( );
            }
        }
    }
    catch ( std::exception& )
    {
        Terminate( );
    }
}

void
SanguisNet::GroupParticipantTcp::Terminate( )
{
    m_BelongsTo->Leave( shared_from_this( ) );
    try
    {
        m_Socket.shutdown( asio::ip::tcp::socket::shutdown_both );
        m_Socket.close( );
    }
    catch ( ... )
    { }
    m_MessageSignal.cancel( );
}
