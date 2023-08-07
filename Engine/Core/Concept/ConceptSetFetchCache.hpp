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

    bool
    NotEmpty( );

    void
    Clear( );

private:
    uint64_t m_CacheHash { };

    std::vector<Ty*> m_CachedConcepts { };

    friend class Concept;
};

template <typename Ty>
void
ConceptSetFetchCache<Ty>::ForEach( auto&& F )
{
    for ( Ty* C : m_CachedConcepts )
    {
        F( C );
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
