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
    LobbyScene( const std::shared_ptr<SanguisNet::ClientGroupParticipant>& Connection, std::string UserName );

protected:
    void ServerMessageCallback( const SanguisNet::Message& Msg );

    std::shared_ptr<SanguisNet::ClientGroupParticipant> m_ServerConnection;

    std::vector<std::shared_ptr<class Text>> m_LobbyMemberText;

    std::shared_ptr<class Canvas> m_UICanvas;

    std::string m_UserName;

    bool m_InLobby = false;
};
