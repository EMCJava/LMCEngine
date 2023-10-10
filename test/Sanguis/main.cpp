#include <Engine/Engine.hpp>
#include <exception>
#include <iostream>

#include <thread>

#include <Engine/Core/Network/NetServer.hpp>
#include <Engine/Core/Network/NetClient.hpp>
#include <spdlog/spdlog.h>

void
ServerT( )
{
    NetServer server;
    server.Setup( NetType::TCP, "127.0.0.1", 12333 );
    server.Listen( 2 );

    for ( int i = 0; i < 2; i++ )
    {
        auto Sock = server.Accept( );

        std::vector<char> Buffer( 1024, 0 );
        Sock->Receive( Buffer );

        spdlog::info( "Received {} bytes", Buffer.size( ) );
        spdlog::info( "\n===\n{}\n===", Buffer.data( ) );
    }
}

void
ClientT( )
{
    NetClient client;
    client.Setup( NetType::TCP, "127.0.0.1", 12333 );

    std::vector<char> Buffer( 1024, 0 );
    strcpy( Buffer.data( ), "Testing, Hello World!!!" );
    Buffer.resize( strlen( Buffer.data( ) ) + 1 );

    client.Send( Buffer );
}

int
main( )
{
    spdlog::set_level( spdlog::level::trace );

    std::thread ServerThread( ServerT );
    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread ClientThread( ClientT );

    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread T( ClientT );
    ClientThread.swap( T );
    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    if ( ServerThread.joinable( ) )
        ServerThread.join( );

    return 0;

    Engine engine;

    try
    {
        while ( !engine.ShouldShutdown( ) )
        {
            engine.Update( );
        }
    }
    catch ( std::exception& e )
    {
        std::cerr << "Uncaught exception: " << e.what( ) << std::endl;
    }

    return 0;
}
