//
// Created by Lo Yu Sum on 2024/03/19.
//


#pragma once

#include "Message.hpp"

namespace SanguisNet
{
class GroupParticipant
{
public:
    virtual ~GroupParticipant( )               = default;
    virtual void Deliver( const Message& Msg ) = 0;
    virtual void Terminate( )                  = 0;
};
}   // namespace SanguisNet