//
// Created by EMCJava on 3/21/2024.
//

#include "ServerManager.hpp"
#include "GroupParticipantTcp.hpp"
#include "ServerSectionAcceptor.hpp"

#include "DataBase/DBController.hpp"

void
SanguisNet::ServerManager::Lunch( asio::io_context& ControlContext )
{
    assert( m_DataBaseController );
    m_ServerSections.clear( );

    m_AuthorizationSection = std::make_shared<SanguisNet::ServerSectionGroupAuth>( 8800 );
    m_MainSection          = std::make_shared<SanguisNet::ServerSectionGroupMain>( 8801 );
    m_EchoSection          = std::make_shared<SanguisNet::ServerSectionGroupEcho>( 8802 );

    auto SharedFromThis = shared_from_this( );

    m_AuthorizationSection->SetManager( SharedFromThis );
    m_MainSection->SetManager( SharedFromThis );
    m_EchoSection->SetManager( SharedFromThis );

    m_ServerSections.insert( { m_AuthorizationSection } );

    for ( const auto& Section : m_ServerSections )
    {
        auto Acceptor = asio::ip::tcp::acceptor( ControlContext,
                                                 { asio::ip::tcp::v4( ), Section->GetSectionPort( ) },
                                                 true );

        co_spawn( ControlContext,
                  SanguisNet::ServerSectionAcceptor<SanguisNet::GroupParticipantTcp>( std::move( Acceptor ), Section ),
                  asio::detached );
    }
}

SanguisNet::ServerManager::ServerManager( std::shared_ptr<DBController> DataBaseController )
{
    m_DataBaseController = std::move( DataBaseController );
}
