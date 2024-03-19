//
// Created by EMCJava on 3/18/2024.
//

#include <iostream>
#include <memory>
#include <string>
#include <ranges>

#include "ServerSectionAcceptor.hpp"
#include "ServerSectionGroupEcho.hpp"
#include "GroupParticipantTcp.hpp"
#include "Message.hpp"
#include "DataBase/DBController.hpp"

std::unique_ptr<DBController> DataBase;

asio::awaitable<void>
MainServerListener( asio::ip::tcp::acceptor acceptor )
{
    using namespace std::chrono_literals;

    auto       SectionGroup  = std::make_shared<SanguisNet::ServerSectionGroupEcho>( );
    const auto LocalEndpoint = acceptor.local_endpoint( );

    asio::steady_timer DeadlineTimer( acceptor.get_executor( ) );

    std::function<void( std::error_code )> TimeoutHandler;
    TimeoutHandler = [ & ]( std::error_code ec ) {
        if ( ec ) return;
        if ( SectionGroup->GetParticipantsCount( ) == 0 )
        {
            std::cout << "[" << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( ) << "] Party releasing..." << std::endl;

            // Release section
            acceptor.close( );
        } else
        {
            std::cout << "[" << LocalEndpoint.address( ).to_string( ) << ":" << LocalEndpoint.port( ) << "] Waiting for a client to connect..." << std::endl;
            DeadlineTimer.expires_after( 10s );
            DeadlineTimer.async_wait( TimeoutHandler );
        }
    };

    while ( true )
    {
        DeadlineTimer.expires_after( 10s );
        DeadlineTimer.async_wait( TimeoutHandler );
        auto Socket = co_await acceptor.async_accept( asio::use_awaitable );
        DeadlineTimer.cancel( );

        const asio::ip::tcp::endpoint remoteEndpoint = Socket.remote_endpoint( );

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

int
main( int argc, char* argv[] )
{
    try
    {
        DataBase = std::make_unique<DBController>( );

        {
            using namespace sqlite_orm;

            auto CheckUser = User::MakeUser( "Player1", "1" );
            auto result    = DataBase->GetStorage( ).select( asterisk<User>( ),
                                                             where( c( &User::UserName ) == CheckUser.UserName
                                                                    and c( &User::PasswordHash ) == CheckUser.PasswordHash ) );
            std::cout << result.size( ) << std::endl;
        }

        auto AuthenticationSections = std::make_shared<SanguisNet::ServerSectionGroupEcho>( 8800 );

        std::set<std::shared_ptr<SanguisNet::ServerSectionGroup>> ServerSections;
        ServerSections.insert( { AuthenticationSections } );

        asio::io_context io_context( 1 );

        for ( const auto& Section : ServerSections )
        {
            auto Acceptor = asio::ip::tcp::acceptor( io_context,
                                                     { asio::ip::tcp::v4( ), Section->GetSectionPort( ) },
                                                     true );

            co_spawn( io_context,
                      SanguisNet::ServerSectionAcceptor<SanguisNet::GroupParticipantTcp>( std::move( Acceptor ), Section ),
                      asio::detached );
        }

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