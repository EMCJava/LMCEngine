//
// Created by Lo Yu Sum on 2024/03/22.
//

#include "ServerSectionGroupLobby.hpp"
#include "ServerSectionGroupMain.hpp"
#include "ServerManager.hpp"

#include "DataBase/DBController.hpp"

void
SanguisNet::ServerSectionGroupMain::HandleMessage( const std::shared_ptr<GroupParticipant>& Participant, SanguisNet::Message& Msg )
{
    std::string_view RequestStr { (char*) Msg.data, Msg.header.length };

    switch ( Msg.header.id )
    {
    case SanguisNet::MessageHeader::ID_GET:
        if ( RequestStr.starts_with( "friend_list" ) )
        {
            auto PageStr = RequestStr.substr( strlen( "friend_list" ) );
            int  Page    = std::stoi( PageStr.data( ) );

            ResponseFriendList( Participant, Page );
        }
        break;

    case SanguisNet::MessageHeader::ID_LOBBY_CONTROL:
        if ( RequestStr.starts_with( "create" ) )
            CreateLobby( Participant );
        else if ( RequestStr.starts_with( "leave" ) )
            LeaveLobby( Participant );
        else if ( RequestStr.starts_with( "join" ) )
        {
            auto UserIDStr = RequestStr.substr( strlen( "join" ) );
            int  UserID    = std::stoi( UserIDStr.data( ) );

            JoinLobby( Participant, UserID );
        } else if ( RequestStr.starts_with( "list" ) )
        {
            // Not in a lobby
            if ( !m_PlayerLobbies.contains( Participant->GetParticipantID( ) ) ) break;

            const auto& Lobby           = m_PlayerLobbies[ Participant->GetParticipantID( ) ];
            const auto& ParticipantsSet = Lobby->GetParticipantsSet( );
            std::string Result;
            for ( const auto& P : ParticipantsSet )
            {
                Result += GetParticipantName( P ) + '\n';
            }
            Participant->Deliver( SanguisNet::Message::FromString( Result, MessageHeader::ID_INFO ) );
        } else if ( m_PlayerLobbies.contains( Participant->GetParticipantID( ) ) )
        {
            m_PlayerLobbies[ Participant->GetParticipantID( ) ]->HandleMessage( Participant, Msg );
        }
        break;

    default:
        Participant->Terminate( );
    }
}

void
SanguisNet::ServerSectionGroupMain::ResponseFriendList( const std::shared_ptr<GroupParticipant>& Participant, int Page )
{
    constexpr int FriendPrePage = MaxNamesPreMessage;
    using namespace sqlite_orm;

    auto FriendList = m_Manager.lock( )->GetDBController( )->GetStorage( ).select(
        columns( &User::UserName ),
        inner_join<UserRelationship>( on(
            c( &UserRelationship::RelatingUserId ) == Participant->GetParticipantID( )
            and c( &UserRelationship::RelatedUserId ) == &User::ID
            and c( &UserRelationship::Type ) == (int) UserRelationship::Friend ) ),
        where( exists( select( columns( &UserRelationship::RelatedUserId ),
                               from<UserRelationship>( ),
                               where( c( &UserRelationship::RelatingUserId ) == &User::ID
                                      and c( &UserRelationship::RelatedUserId ) == Participant->GetParticipantID( )
                                      and c( &UserRelationship::Type ) == (int) UserRelationship::Friend ) ) ) ),
        order_by( &User::UserName ).asc( ),
        limit( FriendPrePage, offset( FriendPrePage * Page ) ) );

    std::string Response;
    for ( int i = 0; i < FriendList.size( ); ++i )
    {
        Response += std::get<0>( FriendList[ i ] ) + '\n';
    }

    Participant->Deliver( SanguisNet::Message::FromString( Response, MessageHeader::ID_RESULT ) );
}

void
SanguisNet::ServerSectionGroupMain::Leave( const std::shared_ptr<GroupParticipant>& Participant )
{
    ServerSectionGroup::Leave( Participant );
    LeaveLobby( Participant );
}

void
SanguisNet::ServerSectionGroupMain::CreateLobby( const std::shared_ptr<GroupParticipant>& Participant )
{
    // Already in a lobby
    if ( m_PlayerLobbies.contains( Participant->GetParticipantID( ) ) ) return;

    auto NewLobby = std::make_shared<ServerSectionGroupLobby>( 0 );
    NewLobby->Join( Participant );
    NewLobby->SetManager( m_Manager );
    m_PlayerLobbies.insert( { Participant->GetParticipantID( ), std::move( NewLobby ) } );
    Participant->Deliver( SanguisNet::Message::FromString( "Created", MessageHeader::ID_RESULT ) );
}

void
SanguisNet::ServerSectionGroupMain::JoinLobby( const std::shared_ptr<GroupParticipant>& Participant, int LobbyUserID )
{
    // Not joining "this" lobby
    if ( LobbyUserID == Participant->GetParticipantID( ) ) return;

    // No such lobby
    if ( !m_PlayerLobbies.contains( LobbyUserID ) ) return;

    auto NewLobby = m_PlayerLobbies[ LobbyUserID ];

    // Lobby full
    if ( NewLobby->GetParticipantsCount( ) >= MaxParticipantPerSection )
    {
        Participant->Deliver( SanguisNet::Message::FromString( "LobbyFull", MessageHeader::ID_RESULT ) );
        return;
    }

    // Lobby disable joining
    if ( !NewLobby->CanJoin() )
    {
        Participant->Deliver( SanguisNet::Message::FromString( "CannotJoin", MessageHeader::ID_RESULT ) );
        return;
    }

    auto& OldLobby = m_PlayerLobbies[ Participant->GetParticipantID( ) ];
    if ( OldLobby != nullptr )
    {
        OldLobby->Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participant ) + " Left", MessageHeader::ID_INFO ) );
        OldLobby->Leave( Participant );
    }

    NewLobby->Join( Participant );
    OldLobby = std::move( NewLobby );

    OldLobby->Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participant ) + " Entered", MessageHeader::ID_INFO ) );
}

void
SanguisNet::ServerSectionGroupMain::LeaveLobby( const std::shared_ptr<GroupParticipant>& Participant )
{
    // Not in a lobby
    if ( !m_PlayerLobbies.contains( Participant->GetParticipantID( ) ) ) return;

    auto& Lobby = m_PlayerLobbies[ Participant->GetParticipantID( ) ];
    Lobby->Broadcast( SanguisNet::Message::FromString( GetParticipantName( Participant ) + " Left", MessageHeader::ID_INFO ) );
    Lobby->Leave( Participant );
    m_PlayerLobbies.erase( Participant->GetParticipantID( ) );
}
