//
// Created by EMCJava on 3/22/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

namespace SanguisNet
{
class ServerSectionGroupLobby : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    void HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, const Message& Msg ) override;
};
}   // namespace SanguisNet
