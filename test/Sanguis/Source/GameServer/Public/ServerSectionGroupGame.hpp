//
// Created by EMCJava on 3/24/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

#include <array>

namespace SanguisNet
{
class ServerSectionGroupGame : public ServerSectionGroup
{
public:
    using ServerSectionGroup::ServerSectionGroup;

    virtual void Join( const std::shared_ptr<GroupParticipant>& Participant ) override;

    virtual void Leave( const std::shared_ptr<GroupParticipant>& Participant ) override;

    void HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, Message& Msg ) override;

    std::span<std::shared_ptr<GroupParticipant>> GetParticipants( );

protected:
    size_t m_FixedParticipantsCount = 0;

    std::array<std::shared_ptr<GroupParticipant>, MaxParticipantPerSection> m_FixedParticipants;
};
}   // namespace SanguisNet