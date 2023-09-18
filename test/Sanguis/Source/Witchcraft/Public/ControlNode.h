// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Effect.h"

/**
 *
 */
class SaControlNode
{
public:
    virtual ~SaControlNode( ) = default;
    SaControlNode( );

    SaEffect GetEffect( )
    {
        SaEffect Result;
        GetEffect( Result );
        return Result;
    }

    virtual void GetEffect( SaEffect& Result ) = 0;
};