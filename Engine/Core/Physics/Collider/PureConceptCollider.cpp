//
// Created by samsa on 11/21/2023.
//

#include "PureConceptCollider.hpp"

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT( PureConceptCollider )
PureConceptCollider::~PureConceptCollider( )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }
}