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
class PhyControllerEntity : public Entity
    , public PhyController
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

    const auto& GetVelocity( ) const noexcept { return m_AccumulatedVelocity; }
    void        SetVelocity( const glm::vec3& Velocity ) noexcept { m_AccumulatedVelocity = Velocity; }

    void
    AddFrameForce( const glm::vec3& FrameForce );

    bool IsOnGround( ) const noexcept { return m_OnGround; }
    bool HasMoved( ) const noexcept { return m_HasMoved; }

    void
    Apply( ) override;

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
    bool m_HasMoved = false;

    uint8_t m_LastFrameCollisionFlag = 0;

    glm::vec3 m_AccumulatedVelocity { };
};
