//
// Created by EMCJava on 3/18/2024.
//

#include <exception>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include "ClientGroupParticipant.hpp"


int
main( int argc, char* argv[] )
{
    try
    {
        if ( argc < 3 )
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        asio::io_context io_context;

        asio::ip::tcp::resolver            resolver( io_context );
        auto                               endpoints = resolver.resolve( argv[ 1 ], argv[ 2 ] );
        SanguisNet::ClientGroupParticipant c( io_context, endpoints );

        {
            std::string login = "Player1|1";
            if ( argc > 3 ) login = argv[ 3 ];
            login[ login.find( '|' ) ] = 0;
            c.Post( SanguisNet::Message::FromString( login, SanguisNet::MessageHeader::ID_LOGIN ) );
        }

        {
            c.Post( SanguisNet::Message::FromString( "friend_list0", SanguisNet::MessageHeader::ID_GET ) );
        }

        std::thread t( [ &io_context ]( ) { io_context.run( ); } );

        std::string line;
        while ( std::cout << "-" << std::flush && std::getline( std::cin, line ) )
        {
            c.Post( SanguisNet::Message::FromString( line, SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
        }

        c.Close( );
        t.join( );
    }
    catch ( std::exception& e )
    {
        std::cerr << "Exception: " << e.what( ) << "\n";
    }

    return 0;
}