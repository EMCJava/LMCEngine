//
// Created by EMCJava on 3/20/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

class DBController;
namespace SanguisNet
{
class ServerSectionGroupAuth : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    void HandleMessage( const std::shared_ptr<class GroupParticipant>& Participant, const Message& Msg ) override;
};
}   // namespace SanguisNet
