//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Math/Random/FastRandom.hpp>

#include <vector>

template <typename Ty>
class ConceptSetFetchCache
{
public:
    void
    ForEach( auto&& F );

    void
    ForEachIndex( auto&& F );

    bool
    NotEmpty( );

    void
    Clear( );

    uint64_t
    GetHash( ) const noexcept;

private:
    uint64_t m_CacheHash { };

    std::vector<std::shared_ptr<Ty>> m_CachedConcepts { };

    friend class Concept;
};

template <typename Ty>
uint64_t
ConceptSetFetchCache<Ty>::GetHash( ) const noexcept
{
    return m_CacheHash;
}

template <typename Ty>
void
ConceptSetFetchCache<Ty>::ForEach( auto&& F )
{
    for ( auto& C : m_CachedConcepts )
    {
        F( C );
    }
}

template <typename Ty>
void
ConceptSetFetchCache<Ty>::ForEachIndex( auto&& F )
{
    for ( size_t i = 0; i < m_CachedConcepts.size( ); ++i )
    {
        F( i, m_CachedConcepts[ i ] );
    }
}


template <typename Ty>
bool
ConceptSetFetchCache<Ty>::NotEmpty( )
{
    return !m_CachedConcepts.empty( );
}

template <typename Ty>
void
ConceptSetFetchCache<Ty>::Clear( )
{
    m_CacheHash = 0;
    m_CachedConcepts.clear( );
}
