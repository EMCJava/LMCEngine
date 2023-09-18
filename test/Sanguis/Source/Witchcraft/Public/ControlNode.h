#pragma once

#include "Effect.h"

/**
 *
 */
class SaControlNode
{
public:
    virtual ~SaControlNode( ) = default;
    SaControlNode( )          = default;

    SaEffect GetEffect( )
    {
        SaEffect Result;
        GetEffect( Result );
        return Result;
    }

    virtual void GetEffect( SaEffect& Result ) = 0;
};