//
// Created by Lo Yu Sum on 2024/03/22.
//

#include "ServerSectionGroupMain.hpp"
#include "ServerManager.hpp"
#include "DataBase/DBController.hpp"
void
SanguisNet::ServerSectionGroupMain::HandleMessage( const std::shared_ptr<GroupParticipant>& Participant, const SanguisNet::Message& Msg )
{
    if ( Msg.header.id == SanguisNet::MessageHeader::ID_GET )
    {
        std::string RequestStr { (char*) Msg.data, Msg.header.length };
        if ( RequestStr.starts_with( "friend_list" ) )
        {
            auto PageStr = RequestStr.substr( strlen( "friend_list" ) );
            int  Page    = std::stoi( PageStr );

            ResponseFriendList( Participant, Page );
        }
    }
}

void
SanguisNet::ServerSectionGroupMain::ResponseFriendList( const std::shared_ptr<GroupParticipant>& Participant, int Page )
{
    constexpr int FriendPrePage = 3;
    static_assert( FriendPrePage * User::MaxNameLength <= SanguisNet::MessageDataLength );
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
        Response += std::get<0>( FriendList[ i ] );
        Response.resize( ( i + 1 ) * User::MaxNameLength );
    }

    Participant->Deliver( SanguisNet::Message::FromString( Response, MessageHeader::ID_RESULT ) );
}
