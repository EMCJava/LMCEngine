//
// Created by EMCJava on 3/22/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

#include <unordered_map>

namespace SanguisNet
{
class ServerSectionGroupLobby : public ServerSectionGroup
{
    struct ParticipantStats {
        bool Ready = false;
    };

public:
    using ServerSectionGroup::ServerSectionGroup;

    virtual void Join( const std::shared_ptr<GroupParticipant>& Participant ) override;

    virtual void Leave( const std::shared_ptr<GroupParticipant>& Participant ) override;

    bool CanJoin( ) const noexcept { return m_AllowNewParticipants; }

    void HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, Message& Msg ) override;

protected:
    int                                       m_ReadyCount = 0;
    std::unordered_map<int, ParticipantStats> m_LobbyParticipants;

    bool m_AllowNewParticipants = true;
    bool m_CanCancelReady       = true;
};
}   // namespace SanguisNet
