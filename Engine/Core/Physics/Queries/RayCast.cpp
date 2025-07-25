//
// Created by Lo Yu Sum on 2024/01/26.
//

#include "RayCast.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>

#include <PxPhysicsAPI.h>

RayCast::HitInfo
RayCast::Cast( )
{
    physx::PxScene&        Scene = Engine::GetEngine( )->GetPhysicsEngine( )->GetScene( );
    physx::PxRaycastBuffer PhysxCastResult;

    HitInfo Result;
    {
        const auto Lock = Engine::GetEngine( )->GetPhysicsThreadLock( );
        Result.HasHit   = Scene.raycast( *(physx::PxVec3*) &RayToCast.RayOrigin,
                                         *(physx::PxVec3*) &RayToCast.UnitDirection,
                                         RayToCast.MaxDistance, PhysxCastResult );
    }
    if ( Result.HasHit )
    {
        static_assert( sizeof( decltype( Result.HitPosition ) ) == sizeof( PhysxCastResult.block.position ) );
        static_assert( sizeof( decltype( Result.HitNormal ) ) == sizeof( PhysxCastResult.block.normal ) );
        Result.HitPosition = *(glm::vec3*) &PhysxCastResult.block.position;
        Result.HitNormal   = *(glm::vec3*) &PhysxCastResult.block.normal;
        Result.HitUserData = (class PureConcept*) PhysxCastResult.block.actor->userData;
        Result.HitDistance = PhysxCastResult.block.distance;
    }

    return Result;
}

RayCast::HitInfo
RayCast::Cast( const RayCast::Ray& RayToCast )
{
    static_assert( sizeof( RayCast ) == sizeof( RayCast::Ray ) );
    return ( (RayCast*) &RayToCast )->Cast( );
}

RayCast::HitInfo
RayCast::Cast( PureConceptPerspectiveCamera* RayFromCamera, FloatTy Distance )
{
    RayCast RayToCast;
    RayToCast.RayToCast = CalculateRay( RayFromCamera, Distance );

    return RayToCast.Cast( );
}

RayCast::Ray
RayCast::CalculateRay( PureConceptPerspectiveCamera* RayFromCamera, FloatTy Distance )
{
    Ray RayToCast;
    RayToCast.RayOrigin     = RayFromCamera->GetCameraPosition( );
    RayToCast.UnitDirection = RayFromCamera->GetCameraFacing( );
    RayToCast.MaxDistance   = Distance;

    return RayToCast;
}
