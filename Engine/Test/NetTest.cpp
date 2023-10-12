#include <catch2/catch_test_macros.hpp>

#include <Engine/Core/Network/NetServer.hpp>
#include <Engine/Core/Network/NetClient.hpp>

#include <cstdint>

inline char TCPTestStr[] = "Testing, TCP Hello World!!!";
inline char UDPTestStr[] = "Testing, UDP Hello World!!!";

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
ClientUDPT( )
{
    NetServer client;
    client.Setup( NetType::UDP, "127.0.0.1", 12334 );

    std::vector<char> Buffer( 1024, 0 );
    strcpy( Buffer.data( ), UDPTestStr );
    Buffer.resize( strlen( Buffer.data( ) ) + 1 );

    NetController::NetPoint Address;
    Address.IP   = "127.0.0.1";
    Address.Port = 12333;

    client.SendTo( Buffer, Address );
}

TEST_CASE( "TCP", "[TCP]" )
{
    NetController::NetGlobalInitialize( );

    SECTION( "TCP Communication" )
    {
        NetServer server;
        server.Setup( NetType::TCP, "127.0.0.1", 12333 );
        server.Listen( 1 );

        for ( int i = 0; i < 10; ++i )
        {
            std::thread ClientThread( ClientTCPT );
            auto        Sock = server.Accept( );

            std::vector<char> Buffer( 1024, 0 );
            Sock->Receive( Buffer );

            REQUIRE( strcmp( Buffer.data( ), TCPTestStr ) == 0 );

            if ( ClientThread.joinable( ) ) ClientThread.join( );
        }
    }

    NetController::NetGlobalShutdown( );
}

 TEST_CASE( "UDP", "[UDP]" )
{
    NetController::NetGlobalInitialize( );

    SECTION( "UDP Communication" )
    {
        NetServer server;
        server.Setup( NetType::UDP, "127.0.0.1", 12333 );

        for ( int i = 0; i < 10; ++i )
        {
            std::vector<char> Buffer( 1024, 0 );

            NetController::NetPoint Address;
            std::thread ClientThread( ClientUDPT );
            server.ReceiveFrom( Buffer, Address );

            if ( Buffer.size( ) )
            {
                REQUIRE( strcmp( Buffer.data( ), UDPTestStr ) == 0 );
                i++;
            }

            if ( ClientThread.joinable( ) ) ClientThread.join( );
        }
    }

    NetController::NetGlobalShutdown( );
 }