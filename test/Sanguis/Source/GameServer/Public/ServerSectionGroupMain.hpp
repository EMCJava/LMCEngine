//
// Created by Lo Yu Sum on 2024/03/22.
//


#pragma once

#include "ServerSectionGroup.hpp"

#include <map>

namespace SanguisNet
{
class GroupParticipant;
class ServerSectionGroupMain : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    void HandleMessage( const std::shared_ptr<GroupParticipant>& Participant, Message& Msg ) override;

    virtual void Leave( const std::shared_ptr<GroupParticipant>& Participant ) override;

private:
    void ResponseFriendList( const std::shared_ptr<GroupParticipant>& Participant, int Page );

    void CreateLobby( const std::shared_ptr<GroupParticipant>& Participant );
    void JoinLobby( const std::shared_ptr<GroupParticipant>& Participant, int LobbyUserID );
    void LeaveLobby( const std::shared_ptr<GroupParticipant>& Participant );

protected:
    std::map<int, std::shared_ptr<class ServerSectionGroupLobby>> m_PlayerLobbies;
};
}   // namespace SanguisNet
