//
// Created by samsa on 1/20/2024.
//

#pragma once

#include "PhyController.hpp"

#include <Engine/Core/Scene/Entity/Entity.hpp>

/*
 *
 * A PhyController with common utilities (gravity, friction, etc.)
 *
 * */
class PhyControllerEntity : public PhyController
    , public Entity
{
    DECLARE_CONCEPT( PhyControllerEntity, Entity )
public:
    /*
     *
     * Move with accumulated velocity and gravity
     *
     * */
    physx::PxControllerCollisionFlags
    Move( FloatTy DeltaTime );

    /*
     *
     * One time velocity, reset after calling @Move
     *
     * */
    void
    AddFrameVelocity( const glm::vec3& FrameVelocity );

    void
    AddFrameForce( const glm::vec3& FrameForce );

    bool IsOnGround( ) const noexcept { return m_OnGround; }

protected:
    FloatTy m_Gravity     = -9.81 * 2;
    FloatTy m_MaxVelocity = 60;
    FloatTy m_Friction    = 0.0001f;

    /*
     *
     * Reset after every frame
     *
     * */
    glm::vec3 m_FrameForce { };
    glm::vec3 m_FrameVelocity { };

    bool m_OnGround = false;

    glm::vec3 m_AccumulatedVelocity { };
};
