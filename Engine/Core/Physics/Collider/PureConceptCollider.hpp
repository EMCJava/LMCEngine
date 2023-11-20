//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>

namespace physx
{
class PxRigidActor;
}

class PureConceptCollider : public PureConcept
{
    DECLARE_CONCEPT( PureConceptCollider, PureConcept )

public:
    PureConceptCollider( PhysicsEngine* PhyEngine )
        : m_PhyEngine( PhyEngine )
    { }

    physx::PxRigidActor*
    GetRigidBodyHandle( ) { return m_RigidActor; }

protected:
    /*
     *
     * Physx
     *
     * */
    PhysicsEngine*       m_PhyEngine  = nullptr;
    physx::PxRigidActor* m_RigidActor = nullptr;
};
