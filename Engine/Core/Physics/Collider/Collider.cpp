//
// Created by samsa on 11/21/2023.
//

#include "Collider.hpp"

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT( PureConceptCollider )
Collider::~Collider( )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }
}

Collider::Collider( )
{
    m_PhyEngine = Engine::GetEngine( )->GetPhysicsEngine( );
}
