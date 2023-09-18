#include "Effect.h"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_SIMPLE_IMGUI_TYPE( SaEffect, IsModifier, Iteration, ElementCount )

SaEffect::SaEffect( bool IsModifier, Element Element, int ElementDensity )
    : IsModifier( IsModifier )
{
    ElementCount[ Element ] = ElementDensity;
}
