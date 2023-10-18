//
// Created by loyus on 7/4/2023.
//

#pragma once

#include "ConceptCore.hpp"
#include "PureConcept.hpp"
#include "ConceptSetFetchCache.hpp"

#include <memory>
#include <vector>
#include <algorithm>

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

    template <class ConceptType = PureConcept>
    std::shared_ptr<ConceptType>
    GetConceptAt( size_t SubConceptIndex ) const;

    [[nodiscard]] size_t
    GetSubConceptCount( ) const { return m_SubConcepts.size( ); }

    /*
     *
     * Returns the first matching concept
     *
     * */
    template <class ConceptType = PureConcept>
    [[nodiscard]] std::shared_ptr<ConceptType>
    GetConcept( ) const;

    template <class ConceptType = void, class ElementTy = void>
    void
    GetConcepts( std::vector<std::shared_ptr<ElementTy>>& Out, bool CanSearchThrough = true ) const;

    /*
     *
     * Won't update the cache if CacheHash is same as last time (no change to sub-concept)
     *
     * */
    template <class ConceptType>
    void
    GetConcepts( ConceptSetFetchCache<ConceptType>& Out, bool CanSearchThrough = true ) const;

    template <class ConceptType = PureConcept>
    void
    ForEachSubConcept( auto&& Func );

    [[nodiscard]] bool
    HasSubConcept( ) const;

    /*
     *
     * Get ownership of a sub concept
     *
     * */
    bool
    TransferSubConcept( PureConcept* ConceptPtr );

    void
    SetSearchThrough( bool SearchThrough = true );

    /*
     *
     * Should consider edge case when m_SearchThrough == true
     *
     * */
    void
    ResetSubConceptCache( );

private:
    /*
     *
     * Same as GetConcepts but will not clear the vector
     *
     * */
    template <class ConceptType, class ElementTy>
    void
    GetConcepts_Internal( std::vector<std::shared_ptr<ElementTy>>& Out, bool CanSearchThrough = true ) const;

private:
    std::vector<std::shared_ptr<PureConcept>> m_SubConcepts;

    /*
     *
     * If this is set to true, when searching for "a SET of" concepts, it will also check for its sub-concept
     *
     * */
    bool m_SearchThrough = false;
    friend class PureConcept;
};

template <class ConceptType, typename... Args>
std::shared_ptr<ConceptType>
Concept::AddConcept( Args&&... params )
{
    ResetSubConceptCache( );
    static_assert( ConceptType::template CanCastS<PureConcept>( ) );

    auto Result = ConceptCasting<ConceptType>( m_SubConcepts.emplace_back( std::make_shared<ConceptType>( std::forward<Args>( params )... ) ) );

    Result->m_BelongsTo = this;
    Result->SetRuntimeName( Result->GetClassName( ) );
    return Result;
}

template <class ConceptType>
std::shared_ptr<ConceptType>
Concept::GetConcept( ) const
{
    for ( auto& Concept : m_SubConcepts )
    {
        if ( Concept->CanCastV( ConceptType::TypeID ) )
        {
            return ConceptCasting<ConceptType>( Concept );
        }
    }

    return nullptr;
}

template <class ConceptType>
std::shared_ptr<ConceptType>
Concept::GetConceptAt( size_t SubConceptIndex ) const
{
    REQUIRED_IF( SubConceptIndex < m_SubConcepts.size( ) )
    {
        return ConceptCasting<ConceptType>( m_SubConcepts[ SubConceptIndex ] );
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
            ResetSubConceptCache( );
            return true;
        }
    }

    return false;
}

template <class ConceptType, class ElementTy>
void
Concept::GetConcepts_Internal( std::vector<std::shared_ptr<ElementTy>>& Out, bool CanSearchThrough ) const
{
    const auto CheckSubConcept = [ &Out ]( const std::shared_ptr<PureConcept>& ConceptSharePtr ) {
        const Concept* const ConceptPtr = (Concept*) ConceptSharePtr.get( );

        // Need to check for sub-concept
        if ( ConceptPtr->m_SearchThrough )
        {
            ConceptPtr->GetConcepts_Internal<ConceptType>( Out );
        }
    };

    for ( const auto& ConceptSharePtr : m_SubConcepts )
    {
        if ( ConceptSharePtr->CanCastV( ConceptType::TypeID ) )
        {
            if constexpr ( std::is_same_v<PureConcept, ElementTy> )
            {
                Out.emplace_back( ConceptSharePtr );
            } else
            {
                Out.emplace_back( std::dynamic_pointer_cast<ElementTy>( ConceptSharePtr ) );
            }

            // Can avoid calling virtual functions
            if constexpr ( ConceptType::template CanCastS<Concept>( ) )
            {
                CheckSubConcept( ConceptSharePtr );
                continue;
            }
        }

        // No sub-concept
        if ( !CanSearchThrough || !ConceptSharePtr->CanCastV( Concept::TypeID ) )
        {
            continue;
        }

        CheckSubConcept( ConceptSharePtr );
    }
}

template <class ConceptType, class ElementTy>
void
Concept::GetConcepts( std::vector<std::shared_ptr<ElementTy>>& Out, bool CanSearchThrough ) const
{
    Out.clear( );

    // Not specified
    if constexpr ( std::is_same_v<ConceptType, void> )
    {
        GetConcepts_Internal<ElementTy>( Out, CanSearchThrough );
    } else
    {
        GetConcepts_Internal<ConceptType>( Out, CanSearchThrough );
    }
}

template <class ConceptType>
int
Concept::RemoveConcepts( )
{
    auto PartIt = std::partition( m_SubConcepts.begin( ), m_SubConcepts.end( ),
                                  []( auto& SubConcept ) {
                                      return SubConcept->template CanCastVT<ConceptType>( );
                                  } );

    const auto RemoveCount = std::distance( m_SubConcepts.begin( ), PartIt );
    if ( RemoveCount != 0 ) [[likely]]
    {
        std::for_each( m_SubConcepts.begin( ), PartIt, [ this ]( auto& SubConcept ) {
            TEST( SubConcept->m_BelongsTo == this );
            SubConcept->m_BelongsTo = nullptr;
        } );

        m_SubConcepts.erase( m_SubConcepts.begin( ), PartIt );
        ResetSubConceptCache( );
    }

    return RemoveCount;
}

template <class ConceptType>
void
Concept::GetConcepts( ConceptSetFetchCache<ConceptType>& Out, bool CanSearchThrough ) const
{
    const auto StateHash = m_ConceptsStateHash.SeekUint64( );
    if ( Out.m_CacheHash == StateHash )
    {
        // Concept set not changed
        return;
    }

    Out.m_CacheHash = StateHash;
    GetConcepts<ConceptType>( Out.m_CachedConcepts, CanSearchThrough );
}

template <class ConceptType>
void
Concept::ForEachSubConcept( auto&& Func )
{
    for ( auto& ConceptSharePtr : m_SubConcepts )
    {
        if ( std::is_same_v<PureConcept, ConceptType> || ConceptSharePtr->CanCastV( ConceptType::TypeID ) )
        {
            if constexpr ( std::is_same_v<PureConcept, ConceptType> )
            {
                Func( ConceptSharePtr );
            } else
            {
                Func( std::dynamic_pointer_cast<ConceptType>( ConceptSharePtr ) );
            }
        }
    }
}