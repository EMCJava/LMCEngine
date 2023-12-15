//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>

namespace physx
{
class PxRigidActor;
}

class Collider : public ConceptList
{
    DECLARE_CONCEPT( Collider, ConceptList )

public:
    Collider( );

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
