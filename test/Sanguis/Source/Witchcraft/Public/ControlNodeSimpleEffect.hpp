//
// Created by samsa on 9/19/2023.
//

#pragma once

#include <ControlNode.h>

class ControlNodeSimpleEffect : public SaControlNode
{
public:
    ControlNodeSimpleEffect( ) = default;
    ControlNodeSimpleEffect( const SaEffect& Effect );

public:
    void GetEffect( SaEffect& Result ) override
    {
        Result = m_Effect;
    }

private:
    SaEffect m_Effect;
};