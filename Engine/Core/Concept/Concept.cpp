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

void
Concept::Destroy( )
{
    if ( m_BelongsTo != nullptr )
    {
        m_BelongsTo->RemoveConcept( this );
    }
}

bool
Concept::RemoveConcept( PureConcept* ConceptPtr )
{
    for ( auto It = m_SubConcepts.begin( ); It != m_SubConcepts.end( ); ++It )
    {
        if ( It->get( ) == ConceptPtr )
        {
            m_SubConcepts.erase( It );
            m_ConceptsStateHash.NextUint64( );
            return true;
        }
    }

    return false;
}
