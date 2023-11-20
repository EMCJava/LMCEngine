//
// Created by samsa on 11/21/2023.
//

#include "RigidBody.hpp"

#include <Engine/Core/Concept/ConceptRenderable.hpp>

DEFINE_CONCEPT_DS( RigidBody )

RigidBody::RigidBody( )
{
    // For collider or renderable etc.
    SetSearchThrough( true );
}

void
RigidBody::Apply( )
{
    if ( m_OrientationChanged )
    {
        if ( m_Renderable != nullptr )
        {
            m_Renderable->SetShaderUniform( "modelMatrix", GetModelMatrix( ) );
        }
        m_OrientationChanged = false;
    }
}
