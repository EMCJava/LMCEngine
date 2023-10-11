#include <Engine/Engine.hpp>
#include <exception>
#include <iostream>

#include <thread>

#include <Engine/Core/Network/NetServer.hpp>
#include <Engine/Core/Network/NetClient.hpp>
#include <spdlog/spdlog.h>

void
ServerTCPT( )
{
    NetServer server;
    server.Setup( NetType::TCP, "127.0.0.1", 12333 );
    server.Listen( 2 );

    for ( int i = 0; i < 2; ++i )
    {
        auto Sock = server.Accept( );

        std::vector<char> Buffer( 1024, 0 );
        Sock->Receive( Buffer );

        spdlog::info( "Received {} bytes", Buffer.size( ) );
        spdlog::info( "\n===\n{}\n===", Buffer.data( ) );
    }
}

void
ClientTCPT( )
{
    NetClient client;
    client.Setup( NetType::TCP, "127.0.0.1", 12333 );

    std::vector<char> Buffer( 1024, 0 );
    strcpy( Buffer.data( ), "Testing, TCP Hello World!!!" );
    Buffer.resize( strlen( Buffer.data( ) ) + 1 );

    client.Send( Buffer );
}

void
ServerUDPT( )
{
    NetServer server;
    server.Setup( NetType::UDP, "127.0.0.1", 12333 );

    for ( int i = 0; i < 2; )
    {
        std::vector<char> Buffer( 1024, 0 );

        NetController::NetPoint Address;
        server.ReceiveFrom( Buffer, Address );

        if ( Buffer.size( ) )
        {
            spdlog::info( "Received {} bytes", Buffer.size( ) );
            spdlog::info( "\n===\n{}\n===", Buffer.data( ) );
            i++;
        }
    }
}

void
ClientUDPT( )
{
    NetServer client;
    client.Setup( NetType::UDP, "127.0.0.1", 12334 );

    std::vector<char> Buffer( 1024, 0 );
    strcpy( Buffer.data( ), "Testing, UDP Hello World!!!" );
    Buffer.resize( strlen( Buffer.data( ) ) + 1 );

    NetController::NetPoint Address;
    Address.IP   = "127.0.0.1";
    Address.Port = 12333;

    client.SendTo( Buffer, Address );
}

void
TCPTest( )
{
    std::thread ServerThread( ServerTCPT );
    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread ClientThread( ClientTCPT );

    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread T( ClientTCPT );
    ClientThread.swap( T );
    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    if ( ServerThread.joinable( ) )
        ServerThread.join( );
}

void
UDPTest( )
{
    std::thread ServerThread( ServerUDPT );
    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread ClientThread( ClientUDPT );

    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    std::thread T( ClientUDPT );
    ClientThread.swap( T );
    if ( ClientThread.joinable( ) )
        ClientThread.join( );

    if ( ServerThread.joinable( ) )
        ServerThread.join( );
}

int
main( )
{
    spdlog::set_level( spdlog::level::trace );

    NetController::NetGlobalInitialize( );

    TCPTest( );
    UDPTest( );

    NetController::NetGlobalShutdown( );

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
