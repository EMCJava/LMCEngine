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
        if ( argc != 3 )
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        asio::io_context io_context;

        asio::ip::tcp::resolver            resolver( io_context );
        auto                               endpoints = resolver.resolve( argv[ 1 ], argv[ 2 ] );
        SanguisNet::ClientGroupParticipant c( io_context, endpoints );


        {
            SanguisNet::Message LoginMsg { SanguisNet::MessageHeader::ID_LOGIN };

            char login[]           = "Player1|1";
            LoginMsg.header.length = std::strlen( login );
            login[ 7 ]             = 0;

            std::memcpy( LoginMsg.data, login, LoginMsg.header.length );
            c.Post( LoginMsg );
        }

        {
            SanguisNet::Message RequestMsg { SanguisNet::MessageHeader::ID_GET };

            char request_str[]       = "friend_list0";
            RequestMsg.header.length = std::strlen( request_str );

            std::memcpy( RequestMsg.data, request_str, RequestMsg.header.length );
            c.Post( RequestMsg );
        }

        std::thread t( [ &io_context ]( ) { io_context.run( ); } );

        char line[ SanguisNet::MessageDataLength + 1 ];
        while ( std::cout << "-" << std::flush && std::cin.getline( line, SanguisNet::MessageDataLength + 1 ) )
        {
            SanguisNet::Message msg;
            msg.header.length = std::strlen( line );
            std::memcpy( msg.data, line, msg.header.length );
            c.Post( msg );
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