//
// Created by EMCJava on 3/18/2024.
//

#include <iostream>
#include <asio.hpp>

int
main( int argc, char* argv[] )
{
    asio::io_context io;
    std::cout << "Hello, Server!" << std::endl;

    asio::steady_timer t( io, asio::chrono::seconds( 5 ) );
    t.wait( );

    return 0;
}