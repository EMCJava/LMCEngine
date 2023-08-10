//
// Created by loyus on 7/4/2023.
//

#pragma once

#include "ConceptCore.hpp"
#include "PureConcept.hpp"
#include "ConceptSetFetchCache.hpp"

#include <memory>
#include <vector>

/*
 *
 * Virtual Concept system in this engine
 *
 * */
class Concept : public PureConcept
{
    DECLARE_CONCEPT( Concept, PureConcept )

public:
    /*
     *
     * Operations to sub concepts
     *
     * */
    template <class ConceptType, typename... Args>
    ConceptType*
    AddConcept( Args&&... params );

    template <class ConceptType>
    bool
    RemoveConcept( );

    template <class ConceptType>
    int
    RemoveConcepts( );

    template <class ConceptType>
    ConceptType*
    GetConcept( );

    template <class ConceptType>
    void
    GetConcepts( std::vector<ConceptType*>& Out );

    template <class ConceptType>
    void
    GetConcepts( ConceptSetFetchCache<ConceptType>& Out );

    bool
    HasSubConcept( );

private:

    std::vector<std::unique_ptr<PureConcept>> m_SubConcepts;
};

template <class ConceptType, typename... Args>
ConceptType*
Concept::AddConcept( Args&&... params )
{
    m_ConceptsStateHash.NextUint64( );
    return (ConceptType*) ( m_SubConcepts.emplace_back( std::make_unique<ConceptType>( std::forward<Args>( params )... ) ).get( ) );
}

template <class ConceptType>
ConceptType*
Concept::GetConcept( )
{
    for ( const auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            return (ConceptType*) Concept.get( );
        }
    }

    return nullptr;
}

template <class ConceptType>
bool
Concept::RemoveConcept( )
{
    for ( size_t i = 0; i < m_SubConcepts.size( ); ++i )
    {
        if ( m_SubConcepts[ i ]->CanCastV( ConceptType::TypeID ) )
        {
            m_SubConcepts.erase( m_SubConcepts.begin( ) + i );
            m_ConceptsStateHash.NextUint64( );
            return true;
        }
    }

    return false;
}

template <class ConceptType>
void
Concept::GetConcepts( std::vector<ConceptType*>& Out )
{
    Out.clear( );
    for ( const auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            Out.emplace_back( (ConceptType*) Concept.get( ) );
        }
    }
}

template <class ConceptType>
int
Concept::RemoveConcepts( )
{
    int RemovedCount = std::erase_if( m_SubConcepts.begin( ), m_SubConcepts.end( ),
                                      []( auto& SubConcept ) {
                                          return SubConcept->template CanCast<ConceptType>( );
                                      } );

    if ( RemovedCount > 0 ) [[likely]]
    {
        m_ConceptsStateHash.NextUint64( );
    }

    return RemovedCount;
}

template <class ConceptType>
void
Concept::GetConcepts( ConceptSetFetchCache<ConceptType>& Out )
{
    const auto StateHash = m_ConceptsStateHash.SeekUint64( );
    if ( Out.m_CacheHash == StateHash )
    {
        // Concept set not changed
        return;
    }

    Out.m_CacheHash = StateHash;
    Out.m_CachedConcepts.clear( );
    for ( const auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            Out.m_CachedConcepts.emplace_back( (ConceptType*) Concept.get( ) );
        }
    }
}