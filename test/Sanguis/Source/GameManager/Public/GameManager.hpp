//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <thread>

namespace asio
{
class io_context;
}   // namespace asio
namespace SanguisNet
{
class ClientGroupParticipant;
}

class GameManager : public ConceptList
{
    DECLARE_CONCEPT( GameManager, ConceptList )

public:
    GameManager( );

    void
    TestInvokable( );

private:

    /*
     *
     * Scenes
     *
     * */
    std::shared_ptr<class ConceptList> m_WandEditorScene;

    std::shared_ptr<asio::io_context>                   m_IOContext;
    std::shared_ptr<void>                               m_IOContextHolder;
    std::shared_ptr<SanguisNet::ClientGroupParticipant> m_ServerConnection;
    std::unique_ptr<std::thread>                        m_IOThread;

    ENABLE_IMGUI( GameManager )
};