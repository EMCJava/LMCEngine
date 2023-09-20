//
// Created by samsa on 9/20/2023.
//

#pragma once

#include <ControlNode.h>

class SaControlNodePlaceholder : public SaControlNode
{
public:
    SaControlNodePlaceholder( ) = default;

public:
    void GetEffect( SaEffect& Result ) override
    {
        Result = SaEffect { };
    }
};