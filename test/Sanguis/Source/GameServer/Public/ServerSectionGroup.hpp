//
// Created by Lo Yu Sum on 2024/03/19.
//


#pragma once

#include "GroupParticipant.hpp"

#include <set>

namespace SanguisNet
{
class ServerSectionGroup
{
public:
    void Join( const std::shared_ptr<GroupParticipant>& Participant );

    void Leave( const std::shared_ptr<GroupParticipant>& participant );

    void Deliver( const Message& Msg ) const;

private:
    std::set<std::shared_ptr<GroupParticipant>> m_Participants;
};
}   // namespace SanguisNet
