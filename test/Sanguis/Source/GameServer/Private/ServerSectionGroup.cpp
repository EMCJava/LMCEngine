//
// Created by Lo Yu Sum on 2024/03/19.
//

#include "ServerSectionGroup.hpp"
#include "ServerManager.hpp"
#include "DataBase/DBController.hpp"

#include <spdlog/spdlog.h>

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
    if ( m_Participants.size( ) > MaxParticipantPerSection )
    {
        spdlog::warn( "Section with port:{} is overcrowded with {} participants", m_SectionPort, m_Participants.size( ) );
    }
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
    if ( Participant == nullptr ) return "";
    return m_Manager.lock( )->GetDBController( )->GetUserNameByID( Participant->GetParticipantID( ) );
}
