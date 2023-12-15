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

class Collider : public PureConcept
{
    DECLARE_CONCEPT( Collider, PureConcept )
public:
    virtual physx::PxRigidActor*
    GenerateActor( std::shared_ptr<class RenderableMeshHitBox>* HitBoxFrame ) = 0;
};
