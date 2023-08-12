//
// Created by loyus on 7/13/2023.
//

#include "ConceptRenderable.hpp"

DEFINE_CONCEPT_DS( ConceptRenderable )

void
ConceptRenderable::SetActiveCamera( class PureConceptCamera* ActiveCamera )
{
    m_ActiveCamera = ActiveCamera;
}
