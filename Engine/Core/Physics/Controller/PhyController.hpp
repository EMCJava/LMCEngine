//
// Created by Lo Yu Sum on 2024/01/17.
//


#pragma once

#include <Engine/Core/Core.hpp>

#include <glm/vec3.hpp>

#include <characterkinematic/PxController.h>

namespace physx
{
class PxCapsuleControllerDesc;
class PxCapsuleController;
class PxMaterial;
}   // namespace physx

class PhyController
{
public:
    PhyController( );
    ~PhyController( );

    void
    CreateController( const glm::vec3& position, FloatTy Height, FloatTy Radius, physx::PxMaterial* Material );

    physx::PxCapsuleController*
    GetController( ) const noexcept { return m_Controller; }

    physx::PxControllerCollisionFlags
    MoveRel( const glm::vec3& Displacement, FloatTy DeltaTime );

    const physx::PxCapsuleControllerDesc*
    GetCapsuleDesc( ) const noexcept { return m_CapsuleDesc.get( ); }

    glm::dvec3
    GetFootPosition( );

protected:
    std::unique_ptr<physx::PxCapsuleControllerDesc> m_CapsuleDesc;
    physx::PxCapsuleController*                     m_Controller = nullptr;
};
