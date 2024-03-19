//
// Created by EMCJava on 3/20/2024.
//

#include "ServerSectionGroupAuth.hpp"
#include "GroupParticipant.hpp"

#include "DataBase/DBController.hpp"

#include <string>

SanguisNet::ServerSectionGroupAuth::ServerSectionGroupAuth( uint16_t Port, std::shared_ptr<DBController> DataBase )
    : ServerSectionGroup( Port )
    , m_DataBase( std::move( DataBase ) )
{ }

void
SanguisNet::ServerSectionGroupAuth::HandleMessage( const std::shared_ptr<GroupParticipant>& Participant, const SanguisNet::Message& Msg )
{
    do
    {
        if ( Msg.header.id != MessageHeader::ID_LOGIN ) break;

        size_t NameLength = std::clamp( std::strlen( (char*) Msg.data ), (size_t) 0, (size_t) Msg.header.length );
        if ( NameLength == 0 || NameLength == Msg.header.length ) break;

        std::string UserName { (char*) Msg.data, NameLength };
        std::string Password { (char*) Msg.data + NameLength + 1, Msg.header.length - NameLength - 1 };

        using namespace sqlite_orm;

        auto CheckUser = User::MakeUser( UserName, Password );
        auto Result    = m_DataBase->GetStorage( ).select( asterisk<User>( ),
                                                           where( c( &User::UserName ) == CheckUser.UserName
                                                                  and c( &User::PasswordHash ) == CheckUser.PasswordHash ) );
        if ( Result.empty( ) ) break;

        SanguisNet::Message SuccessMsg { MessageHeader::ID_RESULT };
        snprintf( (char*) SuccessMsg.data, SanguisNet::MessageDataLength, "Login Success" );
        SuccessMsg.header.length = strlen( (char*) SuccessMsg.data );
        Participant->Deliver( SuccessMsg );
        return;
    } while ( false );

    Participant->Terminate( );
}
