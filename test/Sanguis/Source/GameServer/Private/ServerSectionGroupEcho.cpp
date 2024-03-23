//
// Created by EMCJava on 3/20/2024.
//

#include "ServerSectionGroupEcho.hpp"

void
SanguisNet::ServerSectionGroupEcho::HandleMessage( const std::shared_ptr<GroupParticipant>&, SanguisNet::Message& Msg )
{
    Broadcast( Msg );
}
