//
// Created by Lo Yu Sum on 2024/01/26.
//


#pragma once

#include <Engine/Core/Core.hpp>

#include <glm/vec3.hpp>

class RayCast
{
public:
    struct Ray {
        glm::vec3 RayOrigin;
        glm::vec3 UnitDirection;
        FloatTy   MaxDistance;
    };
};
