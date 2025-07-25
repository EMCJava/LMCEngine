//
// Created by Lo Yu Sum on 2024/03/19.
//


#pragma once

#include "Message.hpp"

#include <memory>

namespace SanguisNet
{
class GroupParticipant
    : public std::enable_shared_from_this<GroupParticipant>
{
public:
    virtual ~GroupParticipant( )               = default;
    virtual void Deliver( const Message& Msg ) = 0;
    virtual void Terminate( )                  = 0;

    auto GetParticipantID( ) const noexcept { return m_ParticipantID; }
    void SetParticipantID( int ID ) noexcept { m_ParticipantID = ID; }

    void TransferSection( std::shared_ptr<class ServerSectionGroup> Group );

protected:
    std::shared_ptr<class ServerSectionGroup> m_BelongsTo;

    int m_ParticipantID = -1;
};
}   // namespace SanguisNet