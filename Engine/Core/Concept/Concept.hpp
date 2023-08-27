//
// Created by loyus on 7/4/2023.
//

#pragma once

#include "ConceptCore.hpp"
#include "PureConcept.hpp"
#include "ConceptSetFetchCache.hpp"

#include <memory>
#include <vector>

#pragma warning( Using dynamic_pointer_cast, performance can be impacted )
#define ConceptCasting std::dynamic_pointer_cast

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
    std::shared_ptr<ConceptType>
    AddConcept( Args&&... params );

    bool
    RemoveConcept( PureConcept* ConceptPtr );

    template <class ConceptType>
    bool
    RemoveConcept( );

    template <class ConceptType>
    int
    RemoveConcepts( );

    template <class ConceptType>
    std::shared_ptr<PureConcept>
    GetConcept( );

    template <class ConceptType>
    void
    GetConcepts( std::vector<std::shared_ptr<PureConcept>>& Out );

    template <class ConceptType>
    void
    GetConcepts( ConceptSetFetchCache<ConceptType>& Out );

    bool
    HasSubConcept( );

private:
    std::vector<std::shared_ptr<PureConcept>> m_SubConcepts;
};

template <class ConceptType, typename... Args>
std::shared_ptr<ConceptType>
Concept::AddConcept( Args&&... params )
{
    m_ConceptsStateHash.NextUint64( );
    static_assert( ConceptType::template CanCastS<PureConcept>( ) );

    auto Result = ConceptCasting<ConceptType>( m_SubConcepts.emplace_back( std::make_shared<ConceptType>( std::forward<Args>( params )... ) ) );

    Result->m_BelongsTo = this;
    return Result;
}

template <class ConceptType>
std::shared_ptr<PureConcept>
Concept::GetConcept( )
{
    for ( auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            return Concept;
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
            TEST( m_SubConcepts[ i ]->m_BelongsTo == this );
            m_SubConcepts[ i ]->m_BelongsTo = nullptr;

            m_SubConcepts.erase( m_SubConcepts.begin( ) + i );
            m_ConceptsStateHash.NextUint64( );
            return true;
        }
    }

    return false;
}

template <class ConceptType>
void
Concept::GetConcepts( std::vector<std::shared_ptr<PureConcept>>& Out )
{
    Out.clear( );
    for ( const auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            Out.emplace_back( Concept );
        }
    }
}

template <class ConceptType>
int
Concept::RemoveConcepts( )
{
    auto PartIt = std::partition( m_SubConcepts.begin( ), m_SubConcepts.end( ),
                                  []( auto& SubConcept ) {
                                      return SubConcept->template CanCast<ConceptType>( );
                                  } );

    const auto RemoveCount = std::distance( PartIt, m_SubConcepts.end( ) );
    if ( RemoveCount != 0 ) [[likely]]
    {
        std::for_each( PartIt, m_SubConcepts.end( ), [ this ]( auto& SubConcept ) {
            TEST( SubConcept->m_BelongsTo == this );
            SubConcept->m_BelongsTo = nullptr;
        } );

        m_SubConcepts.erase( PartIt, m_SubConcepts.end( ) );
        m_ConceptsStateHash.NextUint64( );
    }

    return RemoveCount;
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
    for ( auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            Out.m_CachedConcepts.emplace_back( std::dynamic_pointer_cast<ConceptType>( Concept ) );
        }
    }
}