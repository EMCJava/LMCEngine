//
// Created by EMCJava on 3/24/2024.
//

#include "ServerSectionGroupGame.hpp"

#include <spdlog/spdlog.h>

void
SanguisNet::ServerSectionGroupGame::Join( const std::shared_ptr<GroupParticipant>& Participant )
{
    if ( m_FixedParticipantsCount + 1 > MaxParticipantPerSection )
    {
        spdlog::warn( "ServerSectionGroupGame::Join: too many participants, ignored" );
        return;
    }

    ServerSectionGroup::Join( Participant );
    m_FixedParticipantsCount++;
    *std::find( m_FixedParticipants.begin( ), m_FixedParticipants.end( ), nullptr ) = Participant;
}

void
SanguisNet::ServerSectionGroupGame::Leave( const std::shared_ptr<GroupParticipant>& Participant )
{
    if ( m_FixedParticipantsCount <= 0 )
    {
        spdlog::warn( "ServerSectionGroupGame::Leave: too few participants, ignored" );
        return;
    }

    ServerSectionGroup::Leave( Participant );
    m_FixedParticipantsCount--;
    *std::find( m_FixedParticipants.begin( ), m_FixedParticipants.end( ), Participant ) = nullptr;
}

void
SanguisNet::ServerSectionGroupGame::HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, SanguisNet::Message& Msg )
{
    auto ParticipantIndex = std::distance( m_Participants.begin( ), m_Participants.find( Participants ) );
    static_assert( MaxParticipantPerSection < std::numeric_limits<unsigned char>::max( ) );

    switch ( Msg.header.id )
    {
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_LIST: {
        std::string Response;
        for ( const auto& P : m_FixedParticipants )
            Response += GetParticipantName( P ) + '\n';
        Response.pop_back( );
        Participants->Deliver( SanguisNet::Message::FromString( Response, MessageHeader::ID_GAME_PLAYER_LIST ) );
        return;
    }
    case SanguisNet::MessageHeader::ID_GAME_UPDATE_SELF_COORDINATES:
        Msg.header.id = SanguisNet::MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES;
        SanguisNet::Game::Encoder<SanguisNet::MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> { }( Msg, ParticipantIndex );
        for ( const auto& P : m_Participants )
            if ( P != Participants ) P->Deliver( Msg );
        Msg.header.id = SanguisNet::MessageHeader::ID_GAME_UPDATE_SELF_COORDINATES;
        SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_UPDATE_PLAYER_COORDINATES> { }( Msg );
        break;
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_RECEIVE_DAMAGE:
        SanguisNet::Game::Encoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg, ParticipantIndex );
        Broadcast( Msg );
        SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg );
        break;
    case SanguisNet::MessageHeader::ID_GAME_GUN_FIRE:
        [[fallthrough]];
    case SanguisNet::MessageHeader::ID_GAME_PLAYER_STAT:
        SanguisNet::Game::Encoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg, ParticipantIndex );
        for ( const auto& P : m_Participants )
            if ( P != Participants ) P->Deliver( Msg );
        SanguisNet::Game::Decoder<SanguisNet::MessageHeader::ID_GAME_GUN_FIRE> { }( Msg );
        break;
    }
}

std::span<std::shared_ptr<SanguisNet::GroupParticipant>>
SanguisNet::ServerSectionGroupGame::GetParticipants( )
{
    return { m_FixedParticipants.data( ), m_FixedParticipantsCount };
}
