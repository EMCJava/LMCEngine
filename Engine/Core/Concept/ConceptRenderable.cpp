//
// Created by loyus on 7/13/2023.
//

#include "ConceptRenderable.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>

DEFINE_CONCEPT_DS( ConceptRenderable )

void
ConceptRenderable::SetActiveCamera( class PureConceptCamera* ActiveCamera )
{
    m_ActiveCamera = ActiveCamera;
}

ConceptRenderable::ConceptRenderable( )
{
    SetActiveCamera( Engine::GetEngine( )->GetGlobalResourcePool( )->Get<PureConceptCamera>( "DefaultCamera" ) );
}
