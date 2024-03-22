//
// Created by EMCJava on 3/22/2024.
//

#include "ServerSectionGroupLobby.hpp"

void
SanguisNet::ServerSectionGroupLobby::HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, const SanguisNet::Message& Msg )
{
    Participants->Deliver( Msg );
}
