//
// Created by loyus on 7/4/2023.
//

#include "Concept.hpp"

#include <random>

DEFINE_CONCEPT( Concept )

Concept::~Concept( )
{
    for ( auto& Con : m_SubConcepts )
    {
        TEST( Con->m_BelongsTo == this );
        Con->m_BelongsTo = nullptr;
    }

    spdlog::trace( "{}::~{} -> {}", "Concept", "Concept", fmt::ptr( this ) );
}

bool
Concept::HasSubConcept( )
{
    return !m_SubConcepts.empty( );
}

bool
Concept::RemoveConcept( PureConcept* ConceptPtr )
{
    for ( auto It = m_SubConcepts.begin( ); It != m_SubConcepts.end( ); ++It )
    {
        if ( It->get( ) == ConceptPtr )
        {
            TEST( ConceptPtr->m_BelongsTo == this );
            ConceptPtr->m_BelongsTo = nullptr;

            m_SubConcepts.erase( It );
            m_ConceptsStateHash.NextUint64( );
            return true;
        }
    }

    return false;
}

bool
Concept::TransferSubConcept( PureConcept* ConceptPtr )
{
    if ( ConceptPtr->m_BelongsTo != nullptr && ConceptPtr->m_BelongsTo != this ) [[likely]]
    {
        auto&      OriginalSet = ConceptPtr->m_BelongsTo->m_SubConcepts;
        const auto OccIt       = std::find_if( OriginalSet.begin( ), OriginalSet.end( ),
                                               [ ConceptPtr ]( std::shared_ptr<PureConcept>& SubConcept ) {
                                             return SubConcept.get( ) == ConceptPtr;
                                         } );

        REQUIRED_IF( OccIt != OriginalSet.end( ) )
        {
            m_SubConcepts.emplace_back( std::move( *OccIt ) );
            m_ConceptsStateHash.NextUint64( );

            OriginalSet.erase( OccIt );
            ConceptPtr->m_BelongsTo->m_ConceptsStateHash.NextUint64( );

            ConceptPtr->m_BelongsTo = this;
            return true;
        }
    }

    return false;
}
