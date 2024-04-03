//
// Created by EMCJava on 3/22/2024.
//

#include "ServerSectionGroupLobby.hpp"
#include "ServerSectionGroupGame.hpp"

void
SanguisNet::ServerSectionGroupLobby::HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, SanguisNet::Message& Msg )
{
    switch ( Msg.header.id )
    {
    case MessageHeader::ID_RECAST:
        Broadcast( Msg );
        return;
    case MessageHeader::ID_LOBBY_LIST: {   // Assume m_AllowNewParticipants == false after m_GameSection is created
        std::string Response;
        for ( const auto& P : m_Participants )
            Response += GetParticipantName( P ) + '\n';
        if ( !m_Participants.empty( ) ) Response.pop_back( );
        Participants->Deliver( SanguisNet::Message::FromString( Response, MessageHeader::ID_LOBBY_LIST ) );
        return;
    }
    case MessageHeader::ID_LOBBY_CONTROL:
        if ( Msg.StartWith( "ready" ) )
        {
            auto& Stat = m_LobbyParticipants[ Participants->GetParticipantID( ) ];
            if ( !Stat.Ready )
            {
                ++m_ReadyCount;
                Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participants ) + "Ready", MessageHeader::ID_INFO ) );
            }
            Stat.Ready = true;

            // If all players are ready, start the game
            if ( m_ReadyCount == m_LobbyParticipants.size( ) )
            {
                Broadcast( SanguisNet::Message::FromString( "AllReady", MessageHeader::ID_INFO ) );
                m_AllowNewParticipants = false;

                m_GameSection = std::make_shared<ServerSectionGroupGame>( );
                m_GameSection->SetManager( m_Manager );
                for ( const auto& P : m_Participants )
                    m_GameSection->Join( P );
            }
        } else if ( Msg.StartWith( "cancel_ready" ) )
        {
            auto& Stat = m_LobbyParticipants[ Participants->GetParticipantID( ) ];
            if ( Stat.Ready && m_CanCancelReady )
            {
                --m_ReadyCount;
                Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participants ) + "CancelReady", MessageHeader::ID_INFO ) );
                m_AllowNewParticipants = true;
            } else
            {
                Participants->Deliver( SanguisNet::Message::FromString( "InvalidCancelReady", MessageHeader::ID_RESULT ) );
            }
            Stat.Ready = false;
        } else
        {
            // Pass to next layer (game)
            break;
        }
        return;
    }

    if ( m_GameSection )
    {
        m_GameSection->HandleMessage( Participants, Msg );
    }
}

void
SanguisNet::ServerSectionGroupLobby::Join( const std::shared_ptr<GroupParticipant>& Participant )
{
    assert( m_AllowNewParticipants );
    ServerSectionGroup::Join( Participant );
    m_LobbyParticipants.insert( { Participant->GetParticipantID( ), {} } );
}

void
SanguisNet::ServerSectionGroupLobby::Leave( const std::shared_ptr<GroupParticipant>& Participant )
{
    ServerSectionGroup::Leave( Participant );
    if ( m_GameSection ) m_GameSection->Leave( Participant );
    auto ParticipantIt = m_LobbyParticipants.find( Participant->GetParticipantID( ) );
    if ( ParticipantIt->second.Ready ) --m_ReadyCount;
    m_LobbyParticipants.erase( ParticipantIt );
}
