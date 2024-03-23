//
// Created by EMCJava on 3/22/2024.
//

#include "ServerSectionGroupLobby.hpp"

void
SanguisNet::ServerSectionGroupLobby::HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, SanguisNet::Message& Msg )
{
    switch ( Msg.header.id )
    {
    case MessageHeader::ID_RECAST:
        Broadcast( Msg );
        break;
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
            }
        } else if ( Msg.StartWith( "cancel_ready" ) )
        {
            auto& Stat = m_LobbyParticipants[ Participants->GetParticipantID( ) ];
            if ( Stat.Ready )
            {
                --m_ReadyCount;
                Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participants ) + "CancelReady", MessageHeader::ID_INFO ) );
            }
            Stat.Ready = false;
        }
        break;
    }
}

void
SanguisNet::ServerSectionGroupLobby::Join( const std::shared_ptr<GroupParticipant>& Participant )
{
    ServerSectionGroup::Join( Participant );
    m_LobbyParticipants.insert( { Participant->GetParticipantID( ), {} } );
}

void
SanguisNet::ServerSectionGroupLobby::Leave( const std::shared_ptr<GroupParticipant>& Participant )
{
    ServerSectionGroup::Leave( Participant );
    auto ParticipantIt = m_LobbyParticipants.find( Participant->GetParticipantID( ) );
    if ( ParticipantIt->second.Ready ) --m_ReadyCount;
    m_LobbyParticipants.erase( ParticipantIt );
}
