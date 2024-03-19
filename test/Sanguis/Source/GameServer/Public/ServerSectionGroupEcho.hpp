//
// Created by EMCJava on 3/20/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

namespace SanguisNet
{
class ServerSectionGroupEcho : public ServerSectionGroup
{
public:
    void HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, const Message& Msg ) override;
};
}   // namespace SanguisNet