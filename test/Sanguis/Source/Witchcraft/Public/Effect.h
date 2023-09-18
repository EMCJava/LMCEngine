#pragma once

#include "BasicElement/AllElement.h"

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include <array>

/**
 *
 */
class SaEffect
{
public:
    SaEffect( )  = default;
    ~SaEffect( ) = default;

    SaEffect( bool IsModifier, Element Element, int ElementDensity = 1 );

    // Can only modify other SaEffect, not used directly
    bool     IsModifier = true;
    uint32_t Iteration  = 0;

    std::array<uint8_t, NumOfElement> ElementCount { };

    ENABLE_IMGUI( SaEffect )
};