//
// Created by EMCJava on 4/2/2024.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

namespace SanguisNet
{
class ClientGroupParticipant;
struct Message;
}   // namespace SanguisNet
class LobbyScene : public ConceptList
{
    DECLARE_CONCEPT( LobbyScene, ConceptList )
public:
    LobbyScene( const std::shared_ptr<SanguisNet::ClientGroupParticipant>& Connection );

protected:
    void ServerMessageCallback( const SanguisNet::Message& Msg );

    std::shared_ptr<SanguisNet::ClientGroupParticipant> m_ServerConnection;

    bool m_InLobby = false;
};
