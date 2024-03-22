//
// Created by Lo Yu Sum on 2024/03/22.
//


#pragma once

#include "ServerSectionGroup.hpp"

#include <map>

namespace SanguisNet
{
class ServerSectionGroupMain : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    void HandleMessage( const std::shared_ptr<class GroupParticipant>& Participant, const Message& Msg ) override;

private:
    void ResponseFriendList( const std::shared_ptr<class GroupParticipant>& Participant, int Page );

protected:
    std::map<int, std::shared_ptr<class ServerSectionGroupLobby>> m_PlayerLobbies;
};
}   // namespace SanguisNet
