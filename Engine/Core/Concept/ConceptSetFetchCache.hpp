//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Math/Random/FastRandom.hpp>

#include <vector>
#include <memory>

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

    friend class ConceptList;
};

#include "ConceptSetFetchCache.inl"