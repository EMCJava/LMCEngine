//
// Created by Lo Yu Sum on 2024/01/17.
//


#pragma once

#include <Engine/Core/Core.hpp>

#include <glm/vec3.hpp>

namespace physx
{
class PxCapsuleController;
class PxMaterial;
}   // namespace physx

class PhyController
{
public:
    PhyController( ) = default;

    void
    CreateController( const glm::vec3& position, FloatTy Height, FloatTy Radius, physx::PxMaterial* Material );

    physx::PxCapsuleController*
    GetController( ) const noexcept { return m_Controller; }

    void
    MoveRel( const glm::vec3& Velocity, FloatTy DeltaTime );

    glm::dvec3
    GetFootPosition( );

private:
    physx::PxCapsuleController* m_Controller = nullptr;
};
