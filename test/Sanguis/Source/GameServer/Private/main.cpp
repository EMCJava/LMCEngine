//
// Created by EMCJava on 3/18/2024.
//

#include <iostream>
#include <memory>
#include <string>
#include <ranges>

#include "ServerManager.hpp"
#include "DataBase/DBController.hpp"

int
main( int argc, char* argv[] )
{
    try
    {
        auto SM = std::make_shared<SanguisNet::ServerManager>( std::make_shared<DBController>( ) );

        asio::io_context io_context( 1 );
        SM->Lunch( io_context );

        //        asio::signal_set signals( io_context, SIGINT, SIGTERM );
        //        signals.async_wait( [ & ]( auto, auto ) { io_context.stop( ); } );

        io_context.run( );
    }
    catch ( std::exception& e )
    {
        std::cerr << "Exception: " << e.what( ) << "\n";
    }

    return 0;
}