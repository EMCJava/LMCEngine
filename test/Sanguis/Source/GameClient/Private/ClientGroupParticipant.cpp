//
// Created by EMCJava on 3/19/2024.
//

#include "ClientGroupParticipant.hpp"

#include <iostream>

SanguisNet::ClientGroupParticipant::ClientGroupParticipant( asio::io_context& io_context, const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type& EndPoint )
    : m_ControlContext( io_context )
    , m_Socket( io_context )
{
    DoConnect( EndPoint );
}

void
SanguisNet::ClientGroupParticipant::Post( const SanguisNet::Message& msg )
{
    asio::post( m_ControlContext,
                [ this, msg ]( ) {
                    bool Sending = !m_MessageQueue.empty( );
                    m_MessageQueue.push_back( msg );
                    if ( !Sending && m_ConnectionReady )
                    {
                        SendTopMessage( );
                    }
                } );
}

void
SanguisNet::ClientGroupParticipant::Close( )
{
    asio::post( m_ControlContext, [ this ]( ) { Terminate( ); } );
}

void
SanguisNet::ClientGroupParticipant::DoConnect( const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type& EndPoint )
{
    asio::async_connect( m_Socket, EndPoint,
                         [ this ]( std::error_code ec, auto&& ) {
                             if ( !ec )
                             {
                                 m_ConnectionReady = true;
                                 if ( !m_MessageQueue.empty( ) ) SendTopMessage( );

                                 // Start waiting/reading for incoming messages
                                 ReadMessageHeader( );
                             }
                         } );
}

void
SanguisNet::ClientGroupParticipant::ReadMessageHeader( )
{
    asio::async_read( m_Socket,
                      asio::buffer( &m_ReadMsg.header, SanguisNet::MessageHeaderLength ),
                      [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                          if ( !ec )
                          {
                              // TODO: Handle header

                              ReadMessageBody( );
                          } else
                          {
                              Terminate( );
                          }
                      } );
}

void
SanguisNet::ClientGroupParticipant::ReadMessageBody( )
{
    asio::async_read( m_Socket,
                      asio::buffer( m_ReadMsg.data, m_ReadMsg.header.length ),
                      [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                          if ( !ec )
                          {
                              // TODO: Handle message
                              std::cout << std::string_view( (char*) m_ReadMsg.data, m_ReadMsg.header.length ) << std::endl;

                              // Start waiting/reading next message
                              ReadMessageHeader( );
                          } else
                          {
                              Terminate( );
                          }
                      } );
}

void
SanguisNet::ClientGroupParticipant::SendTopMessage( )
{
    asio::async_write( m_Socket, m_MessageQueue.front( ).GetMinimalAsioBuffer( ),
                       [ this ]( std::error_code ec, std::size_t /*length*/ ) {
                           if ( !ec )
                           {
                               m_MessageQueue.pop_front( );
                               if ( !m_MessageQueue.empty( ) )
                               {
                                   SendTopMessage( );
                               }
                           } else
                           {
                               Terminate( );
                           }
                       } );
}

void
SanguisNet::ClientGroupParticipant::Terminate( )
{
    m_Socket.close( );
    m_ConnectionReady = false;

    std::cout << "Connection Terminated" << std::endl;
}