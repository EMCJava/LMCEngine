//
// Created by loyus on 7/4/2023.
//

#include "Concept.hpp"

#include <random>

DEFINE_CONCEPT_DS( Concept )

bool
Concept::HasSubConcept( )
{
    return !m_SubConcepts.empty( );
}