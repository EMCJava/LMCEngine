//
// Created by samsa on 1/20/2024.
//

#include "EntityPhyController.hpp"

physx::PxControllerCollisionFlags
EntityPhyController::Move( FloatTy DeltaTime )
{
    m_AccumulatedVelocity.y += m_Gravity * DeltaTime;
    m_AccumulatedVelocity += m_FrameVelocity;

    m_FrameVelocity = { };

    return MoveRel( m_AccumulatedVelocity * DeltaTime, DeltaTime );
}

void
EntityPhyController::AddFrameVelocity( const glm::vec3& FrameVelocity )
{
    m_FrameVelocity += FrameVelocity;
}
