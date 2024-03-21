//
// Created by Lo Yu Sum on 2024/03/22.
//


#pragma once

#include "ServerSectionGroup.hpp"

namespace SanguisNet
{
class ServerSectionGroupMain : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    void HandleMessage( const std::shared_ptr<class GroupParticipant>& Participant, const Message& Msg ) override;
};
}   // namespace SanguisNet
