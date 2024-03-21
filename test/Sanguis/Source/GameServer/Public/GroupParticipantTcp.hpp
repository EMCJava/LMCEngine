//
// Created by Lo Yu Sum on 2024/03/19.
//


#pragma once

#include "GroupParticipant.hpp"
#include "ServerSectionGroup.hpp"

#include <deque>

namespace SanguisNet
{
class GroupParticipantTcp
    : public GroupParticipant
    , public std::enable_shared_from_this<GroupParticipantTcp>
{
public:
    GroupParticipantTcp( asio::ip::tcp::socket Socket, std::shared_ptr<ServerSectionGroup> BelongsTo );

    void StartListening( );

    void Deliver( const SanguisNet::Message& Msg ) override;

    void Terminate( ) override;

private:
    asio::awaitable<void> ReaderJob( );
    asio::awaitable<void> WriterJob( );

    asio::ip::tcp::socket               m_Socket;
    asio::steady_timer                  m_MessageSignal;
    std::deque<SanguisNet::Message>     m_MessageQueue;
};
}   // namespace SanguisNet