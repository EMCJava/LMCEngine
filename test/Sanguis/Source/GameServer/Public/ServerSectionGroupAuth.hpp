//
// Created by EMCJava on 3/20/2024.
//

#pragma once

#include "ServerSectionGroup.hpp"

class DBController;
namespace SanguisNet
{
class ServerSectionGroupAuth : public ServerSectionGroup
{
public:
    ServerSectionGroupAuth( uint16_t Port, std::shared_ptr<DBController> DataBase );

    void HandleMessage( const std::shared_ptr<class GroupParticipant>& Participant, const Message& Msg ) override;

protected:
    std::shared_ptr<DBController> m_DataBase;
};
}   // namespace SanguisNet
