//
// Created by samsa on 12/14/2023.
//

#include "ConceptList.hpp"

#include <random>

DEFINE_CONCEPT( ConceptList )

ConceptList::~ConceptList( )
{
    for ( auto& Con : m_SubConcepts )
    {
        REQUIRED_IF( Con->m_BelongsTo == this )
        {
            Con->m_BelongsTo = nullptr;
        }
    }

    spdlog::trace( "{}::~{} -> {}", "ConceptList", "ConceptList", fmt::ptr( this ) );
}

bool
ConceptList::RemoveConcept( PureConcept* ConceptPtr )
{
    for ( auto It = m_SubConcepts.begin( ); It != m_SubConcepts.end( ); ++It )
    {
        if ( It->get( ) == ConceptPtr )
        {
            REQUIRED_IF( ConceptPtr->m_BelongsTo == this )
            {
                ConceptPtr->m_BelongsTo = nullptr;
            }

            m_SubConcepts.erase( It );
            ResetSubConceptCache( );
            return true;
        }
    }

    return false;
}

bool
ConceptList::GetOwnership( PureConcept* ConceptPtr )
{
    if ( ConceptPtr->m_BelongsTo != nullptr && ConceptPtr->m_BelongsTo != this ) [[likely]]
    {
        auto&      OriginalSet = ConceptPtr->m_BelongsTo->m_SubConcepts;
        const auto OccIt       = std::find_if( OriginalSet.begin( ), OriginalSet.end( ),
                                               [ ConceptPtr ]( std::shared_ptr<PureConcept>& SubConcept ) {
                                             return SubConcept.get( ) == ConceptPtr;
                                         } );

        REQUIRED_IF( OccIt != OriginalSet.end( ), throw std::runtime_error( "Can't find self in owner concept" ) )
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
ConceptList::SetSearchThrough( bool SearchThrough )
{
    if ( m_SearchThrough != SearchThrough ) [[likely]]
    {
        m_SearchThrough = SearchThrough;
        ResetSubConceptCache( );
    }
}

void
ConceptList::ResetSubConceptCache( )
{
    m_ConceptsStateHash.NextUint64( );
    if ( m_SearchThrough && m_BelongsTo != nullptr )
    {
        m_BelongsTo->ResetSubConceptCache( );
    }
}

void
PureConcept::MoveToFirstAsSubConcept( )
{
    if ( m_BelongsTo != nullptr )
    {
        TEST( m_BelongsTo->CanCastVT<ConceptList>( ) )

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
PureConcept::DetachFromOwner( )
{
    if ( m_BelongsTo != nullptr )
    {
        m_BelongsTo->RemoveConcept( this );
        m_BelongsTo = nullptr;   // not needed, just to be clear
    }
}

void
PureConcept::MoveToLastAsSubConcept( )
{
    if ( m_BelongsTo != nullptr )
    {
        TEST( m_BelongsTo->CanCastVT<ConceptList>( ) )

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
