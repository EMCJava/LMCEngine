//
// Created by EMCJava on 3/18/2024.
//

#include <exception>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include "Message.hpp"

using asio::ip::tcp;

typedef std::deque<SanguisNet::Message> chat_message_queue;

class chat_client
{
public:
    chat_client( asio::io_context&                  io_context,
                 const tcp::resolver::results_type& endpoints )
        : io_context_( io_context )
        , socket_( io_context )
    {
        do_connect( endpoints );
    }

    void write( const SanguisNet::Message& msg )
    {
        asio::post( io_context_,
                    [ this, msg ]( ) {
                        bool write_in_progress = !write_msgs_.empty( );
                        write_msgs_.push_back( msg );
                        if ( !write_in_progress )
                        {
                            do_write( );
                        }
                    } );
    }

    void close( )
    {
        asio::post( io_context_, [ this ]( ) { socket_.close( ); } );
    }

private:
    void do_connect( const tcp::resolver::results_type& endpoints )
    {
        asio::async_connect( socket_, endpoints,
                             [ this ]( std::error_code ec, tcp::endpoint ) {
                                 if ( !ec )
                                 {
                                     std::cout << "Connected to " << socket_.remote_endpoint( ) << std::endl;
                                     do_read_header( );
                                 }
                             } );
    }

    void do_read_header( )
    {
        asio::async_read( socket_,
                          asio::buffer( &read_msg_.header, SanguisNet::MessageHeaderLength ),
                          [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                              if ( !ec )
                              {
                                  std::cout << "Received header: " << read_msg_.header << std::endl;
                                  do_read_body( );
                              } else
                              {
                                  socket_.close( );
                              }
                          } );
    }

    void do_read_body( )
    {
        asio::async_read( socket_,
                          asio::buffer( read_msg_.data, read_msg_.header.length ),
                          [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                              if ( !ec )
                              {
                                  std::cout.write( (char*) read_msg_.data, read_msg_.header.length );
                                  std::cout << std::endl
                                            << read_msg_
                                            << std::endl;

                                  do_read_header( );
                              } else
                              {
                                  socket_.close( );
                              }
                          } );
    }

    void do_write( )
    {
        asio::async_write( socket_, write_msgs_.front( ).GetMinimalAsioBuffer( ),
                           [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                               if ( !ec )
                               {
                                   write_msgs_.pop_front( );
                                   if ( !write_msgs_.empty( ) )
                                   {
                                       do_write( );
                                   }
                               } else
                               {
                                   socket_.close( );
                               }
                           } );
    }

private:
    asio::io_context&   io_context_;
    tcp::socket         socket_;
    SanguisNet::Message read_msg_;
    chat_message_queue  write_msgs_;
};

int
main( int argc, char* argv[] )
{
    try
    {
        if ( argc != 3 )
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        asio::io_context io_context;

        tcp::resolver resolver( io_context );
        auto          endpoints = resolver.resolve( argv[ 1 ], argv[ 2 ] );
        chat_client   c( io_context, endpoints );

        std::thread t( [ &io_context ]( ) { io_context.run( ); } );

        char line[ SanguisNet::MessageDataLength + 1 ];
        while ( std::cin.getline( line, SanguisNet::MessageDataLength + 1 ) )
        {
            SanguisNet::Message msg;
            msg.header.length = std::strlen( line );
            std::memcpy( msg.data, line, msg.header.length );
            c.write( msg );
        }

        c.close( );
        t.join( );
    }
    catch ( std::exception& e )
    {
        std::cerr << "Exception: " << e.what( ) << "\n";
    }

    return 0;
}