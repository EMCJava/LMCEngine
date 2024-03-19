//
// Created by Lo Yu Sum on 2024/03/19.
//

#include "ServerSectionGroup.hpp"

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