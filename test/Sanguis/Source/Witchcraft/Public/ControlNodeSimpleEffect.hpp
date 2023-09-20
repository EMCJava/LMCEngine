//
// Created by samsa on 9/19/2023.
//

#pragma once

#include <ControlNode.h>

class SaControlNodeSimpleEffect : public SaControlNode
{
public:
    SaControlNodeSimpleEffect( ) = default;
    SaControlNodeSimpleEffect( const SaEffect& Effect );

public:
    void GetEffect( SaEffect& Result ) override
    {
        Result = m_Effect;
    }

private:
    SaEffect m_Effect;
};