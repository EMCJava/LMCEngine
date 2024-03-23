//
// Created by EMCJava on 3/20/2024.
//

#include "ServerSectionGroupAuth.hpp"
#include "GroupParticipant.hpp"
#include "ServerManager.hpp"

#include "DataBase/DBController.hpp"

#include <string>

void
SanguisNet::ServerSectionGroupAuth::HandleMessage( const std::shared_ptr<GroupParticipant>& Participant, SanguisNet::Message& Msg )
{
    do
    {
        if ( Msg.header.id != MessageHeader::ID_LOGIN ) break;

        size_t NameLength = std::clamp( std::strlen( (char*) Msg.data ), (size_t) 0, (size_t) Msg.header.length );
        if ( NameLength == 0 || NameLength == Msg.header.length ) break;

        std::string UserName { (char*) Msg.data, NameLength };

        std::string Password;
        if ( Msg.data[ Msg.header.length - 1 ] == 0 /* Can use strlen normally */ )
            Password.assign( (char*) Msg.data + NameLength + 1 );
        else
            Password.assign( (char*) Msg.data + NameLength + 1, Msg.header.length - NameLength - 1 );

        using namespace sqlite_orm;

        auto CheckUser = User::MakeUser( UserName, Password );
        auto Result    = m_Manager.lock( )->GetDBController( )->GetStorage( ).select( asterisk<User>( ),
                                                                                      where( c( &User::UserName ) == CheckUser.UserName
                                                                                             and c( &User::PasswordHash ) == CheckUser.PasswordHash ) );
        if ( Result.empty( ) ) break;
        Participant->SetParticipantID( std::get<0>( Result[ 0 ] ) );

        SanguisNet::Message SuccessMsg { MessageHeader::ID_RESULT };
        snprintf( (char*) SuccessMsg.data, SanguisNet::MessageDataLength, "Login Success" );
        SuccessMsg.header.length = strlen( (char*) SuccessMsg.data );
        Participant->Deliver( SuccessMsg );

        Participant->TransferSection( m_Manager.lock( )->GetMainSection( ) );

        return;
    } while ( false );

    Participant->Terminate( );
}
