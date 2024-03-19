//
// Created by EMCJava on 3/18/2024.
//

#include <iostream>
#include <memory>
#include <string>
#include <ranges>

#include "ServerSectionGroup.hpp"
#include "GroupParticipantTcp.hpp"
#include "Message.hpp"
#include "DataBase/DBController.hpp"

asio::awaitable<void>
listener( asio::ip::tcp::acceptor acceptor )
{
    auto SectionGroup = std::make_shared<SanguisNet::ServerSectionGroup>( );

    for ( ;; )
    {
        auto Socket = co_await acceptor.async_accept( asio::use_awaitable );

        const auto              LocalEndpoint  = acceptor.local_endpoint( );
        asio::ip::tcp::endpoint remoteEndpoint = Socket.remote_endpoint( );

        std::cout << "Client IP: "
                  << remoteEndpoint.address( ).to_string( ) << ":" << remoteEndpoint.port( )
                  << " connecting to "
                  << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( )
                  << std::endl;

        // Using shared_from_this, it won't be deleted
        std::make_shared<SanguisNet::GroupParticipantTcp>(
            std::move( Socket ),
            SectionGroup )
            ->StartListening( );
    }
}

//----------------------------------------------------------------------

int
main( int argc, char* argv[] )
{
    try
    {
        {
            using namespace sqlite_orm;

            DBController DataBase;
            auto         result = DataBase.GetStorage( ).select( columns( &User::ID, &User::UserName ),
                                                                 where( is_equal( &User::UserName, "Player1" ) ) );
            auto         a      = 0;
        }
        std::cout << "Please enter port number(s) to listen on: " << std::flush;
        std::string ports;
        std::getline( std::cin, ports );

        asio::io_context io_context( 1 );

        for ( const auto word : std::views::split( ports, ' ' ) )
        {
            unsigned short port = std::atoi( std::data( word ) );
            co_spawn( io_context,
                      listener( asio::ip::tcp::acceptor( io_context, { asio::ip::tcp::v4( ), port }, true ) ),
                      asio::detached );
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