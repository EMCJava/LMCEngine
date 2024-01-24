//
// Created by samsa on 11/21/2023.
//

#include "RigidBody.hpp"

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

#include <PxPhysicsAPI.h>

DEFINE_CONCEPT( RigidBody )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RigidBody, Entity )

RigidBody::RigidBody( )
{
    // For collider or renderable etc.
    SetSearchThrough( true );
}

RigidBody::~RigidBody( )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }
}