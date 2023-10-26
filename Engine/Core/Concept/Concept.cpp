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
Concept::HasSubConcept( ) const
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
            ResetSubConceptCache( );
            return true;
        }
    }

    return false;
}

bool
Concept::GetOwnership( PureConcept* ConceptPtr )
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
            ResetSubConceptCache( );

            OriginalSet.erase( OccIt );
            ConceptPtr->m_BelongsTo->ResetSubConceptCache( );

            ConceptPtr->m_BelongsTo = this;
            return true;
        }
    }

    return false;
}

void
Concept::SetSearchThrough( bool SearchThrough )
{
    if ( m_SearchThrough != SearchThrough ) [[likely]]
    {
        m_SearchThrough = SearchThrough;
        ResetSubConceptCache( );
    }
}

void
Concept::ResetSubConceptCache( )
{
    m_ConceptsStateHash.NextUint64( );
    if ( m_SearchThrough && m_BelongsTo != nullptr )
    {
        m_BelongsTo->m_ConceptsStateHash.NextUint64( );
    }
}

template <typename ConceptType>
bool
Concept::GetOwnership( std::shared_ptr<ConceptType> ConceptPtr )
{
    if ( GetOwnership( ConceptPtr.get( ) ) )
    {
        return true;
    }

    // Own already
    if ( ConceptPtr->m_BelongsTo != nullptr )
    {
        return false;
    }

    // Not own by any other concept
    m_SubConcepts.emplace_back( std::move( ConceptPtr ) );
    ResetSubConceptCache( );
    ConceptPtr->m_BelongsTo = this;
    return true;
}

void
PureConcept::MoveToFirstAsSubConcept( )
{
    if ( m_BelongsTo != nullptr )
    {
        TEST( m_BelongsTo->CanCastVT<Concept>( ) )

        const auto pivot = std::find_if( m_BelongsTo->m_SubConcepts.begin( ),
                                         m_BelongsTo->m_SubConcepts.end( ),
                                         [ this ]( const std::shared_ptr<PureConcept>& C ) -> bool {
                                             return C.get( ) == this;
                                         } );

        if ( pivot != m_BelongsTo->m_SubConcepts.end( ) )
        {
            std::rotate( m_BelongsTo->m_SubConcepts.begin( ), pivot, pivot + 1 );
            m_BelongsTo->ResetSubConceptCache( );
        }
    }
}

void
PureConcept::MoveToLastAsSubConcept( )
{
    if ( m_BelongsTo != nullptr )
    {
        TEST( m_BelongsTo->CanCastVT<Concept>( ) )

        const auto pivot = std::find_if( m_BelongsTo->m_SubConcepts.begin( ),
                                         m_BelongsTo->m_SubConcepts.end( ),
                                         [ this ]( const std::shared_ptr<PureConcept>& C ) -> bool {
                                             return C.get( ) == this;
                                         } );

        if ( pivot != m_BelongsTo->m_SubConcepts.end( ) )
        {
            std::rotate( pivot, pivot + 1, m_BelongsTo->m_SubConcepts.end( ) );
            m_BelongsTo->ResetSubConceptCache( );
        }
    }
}
