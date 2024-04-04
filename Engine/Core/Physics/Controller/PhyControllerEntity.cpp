//
// Created by samsa on 1/20/2024.
//

#include "PhyControllerEntity.hpp"

#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT_DS( PhyControllerEntity )

physx::PxControllerCollisionFlags
PhyControllerEntity::Move( FloatTy DeltaTime )
{
    m_AccumulatedVelocity.y += m_Gravity * DeltaTime;
    m_AccumulatedVelocity += m_FrameVelocity;

    const auto Displacement     = ( m_AccumulatedVelocity + m_FrameForce ) * DeltaTime;
    const auto PreviousPosition = GetFootPosition( );
    auto       Flag             = MoveRel( Displacement, DeltaTime );
    const auto CurrentPosition  = GetFootPosition( );
    m_HasMoved                  = glm::length( CurrentPosition - PreviousPosition ) > 0.0001f;

    m_FrameForce = m_FrameVelocity = { };
    m_OnGround                     = Flag.isSet( physx::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN );
    if ( m_OnGround )
    {
        // Direct collision
        m_AccumulatedVelocity.y = 0;
    } else if ( Flag.isSet( physx::PxControllerCollisionFlag::Enum::eCOLLISION_UP ) )
    {
        // Direct collision upwards
        m_AccumulatedVelocity.y = 0;
    } else if ( !m_OnGround && Displacement.y < 0 )   // not directly colliding to the ground, but potentially / close enough, do a sphere overlap check
    {
        const auto* Desc = GetCapsuleDesc( );

        float      radius       = Desc->radius * 0.9f;
        const auto FootPosition = m_Controller->getFootPosition( );
        const auto SphereOrigin = FootPosition + physx::PxExtendedVec3 { 0, radius * 0.9f, 0 };

        physx::PxScene&     Scene = Engine::GetEngine( )->GetPhysicsEngine( )->GetScene( );
        physx::PxOverlapHit HitData;

        auto Lock  = Engine::GetEngine( )->GetPhysicsThreadLock( );
        m_OnGround = physx::PxSceneQueryExt::overlapAny( Scene, physx::PxSphereGeometry( radius ), physx::PxTransform { toVec3( SphereOrigin ) }, HitData );
    }

    // Friction
    if ( m_OnGround )
    {
        const auto backupY   = m_AccumulatedVelocity.y;
        const auto fpsFactor = 0.0625f / DeltaTime;
        m_AccumulatedVelocity -= ( m_AccumulatedVelocity * ( 1 - m_Friction ) ) / fpsFactor;
        m_AccumulatedVelocity.y = backupY;   // No friction on vertical axis

        m_AccumulatedVelocity = glm::clamp( m_AccumulatedVelocity, -m_MaxVelocity, m_MaxVelocity );
    }


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

void
PhyControllerEntity::Apply( )
{
    m_LastFrameCollisionFlag = Move( Engine::GetEngine( )->GetDeltaSecond( ) );
}
