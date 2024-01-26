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
    } RayToCast;

    struct HitInfo {
        bool             HasHit      = false;
        FloatTy          HitDistance = 0;
        glm::vec3        HitPosition, HitNormal;
        class RigidBody* HitUserData = nullptr;

        operator bool( ) const noexcept
        {
            return HasHit;
        }
    };

    HitInfo        Cast( );
    static HitInfo Cast( const Ray& RayToCast );
    static HitInfo Cast( class PureConceptPerspectiveCamera* RayFromCamera, FloatTy Distance = 100 );
};
