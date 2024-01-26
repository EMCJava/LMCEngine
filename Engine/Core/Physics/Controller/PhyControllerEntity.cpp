//
// Created by samsa on 1/20/2024.
//

#include "PhyControllerEntity.hpp"

DEFINE_CONCEPT_DS( PhyControllerEntity )

physx::PxControllerCollisionFlags
PhyControllerEntity::Move( FloatTy DeltaTime )
{
    m_AccumulatedVelocity.y += m_Gravity * DeltaTime;
    m_AccumulatedVelocity += m_FrameVelocity;

    auto Flag    = MoveRel( ( m_AccumulatedVelocity + m_FrameForce ) * DeltaTime, DeltaTime );
    m_FrameForce = m_FrameVelocity = { };

    m_OnGround = Flag.isSet( physx::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN );
    if ( m_OnGround || Flag.isSet( physx::PxControllerCollisionFlag::Enum::eCOLLISION_UP ) )
        m_AccumulatedVelocity.y = 0;

    auto UpdatedOri       = (const Orientation&) m_Orientation;
    UpdatedOri.Coordinate = GetFootPosition( );
    UpdateOrientation( UpdatedOri );

    return Flag;
}

void
PhyControllerEntity::AddFrameVelocity( const glm::vec3& FrameVelocity )
{
    m_FrameVelocity += FrameVelocity;
}

void
PhyControllerEntity::AddFrameForce( const glm::vec3& FrameForce )
{
    m_FrameForce += FrameForce;
}
