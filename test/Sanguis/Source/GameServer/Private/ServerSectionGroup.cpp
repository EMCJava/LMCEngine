//
// Created by Lo Yu Sum on 2024/03/19.
//

#include "ServerSectionGroup.hpp"
#include "ServerManager.hpp"
#include "DataBase/DBController.hpp"

void
SanguisNet::ServerSectionGroup::Broadcast( const SanguisNet::Message& Msg )
{
    for ( const auto& Participant : m_Participants )
        Participant->Deliver( Msg );
}

void
SanguisNet::ServerSectionGroup::Join( const std::shared_ptr<GroupParticipant>& Participant )
{
    m_Participants.insert( Participant );
}

void
SanguisNet::ServerSectionGroup::Leave( const std::shared_ptr<GroupParticipant>& participant )
{
    m_Participants.erase( participant );
}

void
SanguisNet::ServerSectionGroup::SetManager( std::weak_ptr<struct ServerManager> Manager )
{
    m_Manager = std::move( Manager );
}

std::string
SanguisNet::ServerSectionGroup::GetParticipantName( const std::shared_ptr<GroupParticipant>& Participant ) const
{
    return m_Manager.lock( )->GetDBController( )->GetUserNameByID( Participant->GetParticipantID( ) );
}

