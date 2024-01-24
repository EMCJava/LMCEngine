//
// Created by Lo Yu Sum on 2024/01/25.
//

#include "EntityRenderable.hpp"

#include <Engine/Core/Concept/ConceptRenderable.hpp>

DEFINE_CONCEPT_DS( EntityRenderable )

EntityRenderable::EntityRenderable( std::shared_ptr<ConceptRenderable> RenderableConcept )
{
    GetOwnership( std::move( RenderableConcept ) );
}

void
EntityRenderable::Apply( )
{
    ( (ConceptRenderable*) GetConceptAt( 0 ).get( ) )->SetShaderUniform( "modelMatrix", m_Orientation.GetModelMatrix( ) );
}
