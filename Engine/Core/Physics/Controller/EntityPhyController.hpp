//
// Created by samsa on 1/20/2024.
//

#pragma once

#include "PhyController.hpp"

/*
 *
 * A PhyController with common utilities (gravity, friction, etc.)
 *
 * */
class EntityPhyController : public PhyController
{


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

protected:
    FloatTy m_Gravity = -9.81;

    /*
     *
     * Reset after every frame
     *
     * */
    glm::vec3 m_FrameVelocity;
    glm::vec3 m_AccumulatedVelocity;
};
