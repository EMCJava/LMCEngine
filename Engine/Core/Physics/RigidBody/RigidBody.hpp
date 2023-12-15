//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>
#include <Engine/Core/Concept/ConceptSetFetchCache.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

namespace physx
{
class PxRigidActor;
}

class RigidBody : public ConceptApplicable
    , protected Orientation   // Update to Orientation need to forward to child, wrapper is needed
{
    DECLARE_CONCEPT( RigidBody, ConceptApplicable )
public:
    RigidBody( );

    void
    Apply( ) override;

    [[nodiscard]] const auto&
    GetRenderable( ) const { return m_Renderable; }

    [[nodiscard]] const auto&
    GetCollider( ) const noexcept { return m_Collider; }

    /*
     *
     * For forwarding changes, assume always after this call
     *
     * */
    Orientation&
    GetOrientation( )
    {
        m_OrientationChanged = true;
        return *static_cast<Orientation*>( this );
    }

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

    bool m_OrientationChanged = true;

    /*
     *
     * There could be two or more renderables, one for the rigid body, and one for the collider
     *
     * */
    ConceptSetFetchCache<class ConceptRenderable> m_RenderableSet;

    // FIXME: Consider making these two unique_ptr, for 1 to 1 relationship
    std::shared_ptr<class ConceptRenderable> m_Renderable;
    std::shared_ptr<class Collider>          m_Collider;

    ENABLE_IMGUI( RigidBody )
};
