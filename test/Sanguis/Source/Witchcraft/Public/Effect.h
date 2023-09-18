// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasicElement/AllElement.h"

#include <array>

/**
 *
 */
class SaEffect
{
public:
    SaEffect( );
    ~SaEffect( );

    // Can only modify other SaEffect, not used directly
    bool     IsModifier = true;
    uint32_t Iteration  = 0;

    std::array<uint8_t, NumOfElement> ElementCount;
};
