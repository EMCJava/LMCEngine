//
// Created by EMCJava on 3/20/2024.
//

#include "ServerSectionGroupEcho.hpp"

void
SanguisNet::ServerSectionGroupEcho::HandleMessage( const std::shared_ptr<GroupParticipant>&, const SanguisNet::Message& Msg )
{
    for ( const auto& Participant : m_Participants )
        Participant->Deliver( Msg );
}
