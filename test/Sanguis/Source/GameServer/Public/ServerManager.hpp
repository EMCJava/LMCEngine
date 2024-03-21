//
// Created by EMCJava on 3/21/2024.
//

#pragma once

#include "ServerSectionGroupAuth.hpp"
#include "ServerSectionGroupEcho.hpp"

#include <memory>

namespace SanguisNet
{
class ServerManager
    : public std::enable_shared_from_this<ServerManager>
{
public:
    ServerManager( std::shared_ptr<class DBController> DataBaseController );

    void Lunch( asio::io_context& ControlContext );

    auto GetAuthSection( ) { return m_AuthorizationSection; }
    auto GetEchoSection( ) { return m_EchoSection; }

protected:
    std::shared_ptr<ServerSectionGroupAuth> m_AuthorizationSection;
    std::shared_ptr<ServerSectionGroupEcho> m_EchoSection;

    std::set<std::shared_ptr<SanguisNet::ServerSectionGroup>> m_ServerSections;

    std::shared_ptr<class DBController> m_DataBaseController;
};
}   // namespace SanguisNet