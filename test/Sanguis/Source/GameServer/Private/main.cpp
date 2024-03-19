//
// Created by EMCJava on 3/18/2024.
//

#include <iostream>
#include <memory>
#include <string>
#include <ranges>

#include "ServerSectionAcceptor.hpp"
#include "ServerSectionGroupAuth.hpp"
#include "GroupParticipantTcp.hpp"
#include "Message.hpp"
#include "DataBase/DBController.hpp"

std::shared_ptr<DBController> DataBase;

int
main( int argc, char* argv[] )
{
    try
    {
        DataBase = std::make_shared<DBController>( );

        {
            using namespace sqlite_orm;

            auto CheckUser = User::MakeUser( "Player1", "1" );
            auto result    = DataBase->GetStorage( ).select( asterisk<User>( ),
                                                             where( c( &User::UserName ) == CheckUser.UserName
                                                                    and c( &User::PasswordHash ) == CheckUser.PasswordHash ) );
            std::cout << result.size( ) << std::endl;
        }

        auto AuthenticationSections = std::make_shared<SanguisNet::ServerSectionGroupAuth>( 8800, DataBase );

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