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

    m_FrameVelocity = { };

    auto Flag = MoveRel( m_AccumulatedVelocity * DeltaTime, DeltaTime );

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
