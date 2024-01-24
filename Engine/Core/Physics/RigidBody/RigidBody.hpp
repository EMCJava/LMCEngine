//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Scene/Entity/Entity.hpp>
#include <Engine/Core/Concept/ConceptApplicable.hpp>
#include <Engine/Core/Concept/ConceptSetCache.hpp>
#include <Engine/Core/Scene/Orientation/OrientationMatrix.hpp>

namespace physx
{
class PxRigidActor;
}

class RigidBody : public Entity
{
    DECLARE_CONCEPT( RigidBody, Entity )
public:
    RigidBody( );

    [[nodiscard]] const auto&
    GetRenderable( ) const noexcept { return m_Renderable; }

    [[nodiscard]] const auto&
    GetCollider( ) const noexcept { return m_Collider; }

    /*
     *
     * For forwarding changes, assume always after this call
     *
     * */
    [[nodiscard]] const OrientationMatrix&
    GetConstOrientation( ) const noexcept
    {
        return m_Orientation;
    }

    /*
     *
     * For forwarding changes, assume always after this call
     *
     * */
    OrientationMatrix&
    GetOrientation( ) noexcept
    {
        return m_Orientation;
    }

    physx::PxRigidActor*
    GetRigidBodyHandle( ) const noexcept { return m_RigidActor; }

protected:
    /*
     *
     * Physx
     *
     * */
    PhysicsEngine*       m_PhyEngine  = nullptr;
    physx::PxRigidActor* m_RigidActor = nullptr;

    // FIXME: Consider making these two unique_ptr, for 1 to 1 relationship
    std::shared_ptr<class ConceptRenderable> m_Renderable;
    std::shared_ptr<class Collider>          m_Collider;

    ENABLE_IMGUI( RigidBody )
};
