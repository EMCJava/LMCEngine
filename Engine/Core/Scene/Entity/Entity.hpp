//
// Created by samsa on 12/15/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class Entity : public ConceptList
    , protected Orientation   // Update to Orientation need to forward to child, wrapper is needed
{
    DECLARE_CONCEPT( Entity, ConceptList )
    ENABLE_IMGUI( Entity )
};
