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
    ServerSectionGroup( uint16_t Port = 0 )
        : m_SectionPort( Port )
    { }

    virtual void Join( const std::shared_ptr<GroupParticipant>& Participant );

    virtual void Leave( const std::shared_ptr<GroupParticipant>& participant );

    size_t GetParticipantsCount( ) const noexcept { return m_Participants.size( ); }

    auto GetSectionPort( ) const noexcept { return m_SectionPort; }

    virtual void HandleMessage( const std::shared_ptr<GroupParticipant>& Participants, const Message& Msg ) { }

protected:
    std::set<std::shared_ptr<GroupParticipant>> m_Participants;

    uint16_t m_SectionPort = 0;
};
}   // namespace SanguisNet
