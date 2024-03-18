//
// Created by EMCJava on 3/18/2024.
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <coroutine>
#include <ranges>
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/detached.hpp>
#include <asio/co_spawn.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/redirect_error.hpp>
#include <asio/signal_set.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/write.hpp>

#include "Message.hpp"

using asio::detached;
using asio::redirect_error;
using asio::ip::tcp;

//----------------------------------------------------------------------

class chat_participant
{
public:
    virtual ~chat_participant( ) { }
    virtual void deliver( const SanguisNet::Message& msg ) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
    void join( chat_participant_ptr participant )
    {
        participants_.insert( participant );
        for ( auto msg : recent_msgs_ )
            participant->deliver( msg );
    }

    void leave( chat_participant_ptr participant )
    {
        participants_.erase( participant );
    }

    void deliver( const SanguisNet::Message& msg )
    {
        recent_msgs_.push_back( msg );
        while ( recent_msgs_.size( ) > max_recent_msgs )
            recent_msgs_.pop_front( );

        for ( auto participant : participants_ )
            participant->deliver( msg );
    }

private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    std::deque<SanguisNet::Message> recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant
    , public std::enable_shared_from_this<chat_session>
{
public:
    chat_session( tcp::socket socket, chat_room& room )
        : socket_( std::move( socket ) )
        , timer_( socket_.get_executor( ) )
        , room_( room )
    {
        timer_.expires_at( std::chrono::steady_clock::time_point::max( ) );
    }

    void start( )
    {
        room_.join( shared_from_this( ) );

        asio::co_spawn( socket_.get_executor( ), [ self = shared_from_this( ) ] { return self->reader( ); }, detached );

        asio::co_spawn( socket_.get_executor( ), [ self = shared_from_this( ) ] { return self->writer( ); }, detached );
    }

    void deliver( const SanguisNet::Message& msg )
    {
        write_msgs_.push_back( msg );
        timer_.cancel_one( );
    }

private:
    asio::awaitable<void> reader( )
    {
        try
        {
            SanguisNet::Message msg;
            while ( true )
            {
                co_await asio::async_read( socket_, asio::buffer( &msg.header, sizeof( msg.header ) ), asio::use_awaitable );
                std::cout << "Received header: [" << msg.header << std::endl;
                co_await asio::async_read( socket_, asio::buffer( &msg.data, msg.header.length ), asio::use_awaitable );
                std::cout << "Received message: [" << msg << std::endl;
                room_.deliver( msg );
                msg.header = { };
            }
        }
        catch ( std::exception& )
        {
            stop( );
        }
    }

    asio::awaitable<void> writer( )
    {
        try
        {
            while ( socket_.is_open( ) )
            {
                if ( write_msgs_.empty( ) )
                {
                    asio::error_code ec;
                    co_await timer_.async_wait( redirect_error( asio::use_awaitable, ec ) );
                } else
                {
                    co_await asio::async_write( socket_, write_msgs_.front( ).GetMinimalAsioBuffer( ), asio::use_awaitable );
                    write_msgs_.pop_front( );
                }
            }
        }
        catch ( std::exception& )
        {
            stop( );
        }
    }

    void stop( )
    {
        room_.leave( shared_from_this( ) );
        socket_.close( );
        timer_.cancel( );
    }

    tcp::socket                     socket_;
    asio::steady_timer              timer_;
    chat_room&                      room_;
    std::deque<SanguisNet::Message> write_msgs_;
};

//----------------------------------------------------------------------

asio::awaitable<void>
listener( tcp::acceptor acceptor )
{
    chat_room room;

    for ( ;; )
    {
        std::make_shared<chat_session>(
            co_await acceptor.async_accept( asio::use_awaitable ),
            room )
            ->start( );
    }
}

//----------------------------------------------------------------------

int
main( int argc, char* argv[] )
{
    try
    {
        std::cout << "Please enter port number(s) to listen on: " << std::flush;
        std::string ports;
        std::getline( std::cin, ports );

        asio::io_context io_context( 1 );

        for ( const auto word : std::views::split( ports, ' ' ) )
        {
            unsigned short port = std::atoi( std::data( word ) );
            co_spawn( io_context,
                      listener( tcp::acceptor( io_context, { tcp::v4( ), port } ) ),
                      detached );
        }

        asio::signal_set signals( io_context, SIGINT, SIGTERM );
        signals.async_wait( [ & ]( auto, auto ) { io_context.stop( ); } );

        io_context.run( );
    }
    catch ( std::exception& e )
    {
        std::cerr << "Exception: " << e.what( ) << "\n";
    }

    return 0;
}