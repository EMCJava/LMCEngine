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
            c.Post( SanguisNet::Message::FromString( "0", SanguisNet::MessageHeader::ID_FRIEND_LIST ) );
        }

        std::thread t( [ &io_context ]( ) { io_context.run( ); } );

        auto Mode = SanguisNet::MessageHeader::ID_LOBBY_CONTROL;

        std::string line;
        while ( std::cout << "-" << std::flush && std::getline( std::cin, line ) )
        {
            if ( line == "lc" )
            {
                Mode = SanguisNet::MessageHeader::ID_LOBBY_CONTROL;
                continue;
            } else if ( line == "uc" )
            {
                Mode = SanguisNet::MessageHeader::ID_GAME_UPDATE_SELF_COORDINATES;
                continue;
            } else if ( line == "dd" )
            {
                c.Post( SanguisNet::Message::FromString( std::to_string( 0 ) + '\0' + std::to_string( 54 ), SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE ) );
                continue;
            }

            c.Post( SanguisNet::Message::FromString( line, Mode ) );
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