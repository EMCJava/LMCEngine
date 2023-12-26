//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Math/Random/FastRandom.hpp>

#include <vector>
#include <memory>

template <typename Container>
concept IsSharedPtrOrWeakPtr = std::is_same_v<Container, std::shared_ptr<typename Container::element_type>>
    || std::is_same_v<Container, std::weak_ptr<typename Container::element_type>>;

template <IsSharedPtrOrWeakPtr Container>
class ConceptSetCache
{
public:
    /*
     *
     * Using Container type
     *
     * */
    void
    ForEachOriginal( auto&& F );

    /*
     *
     * Using raw-pointer type
     *
     * */
    void
    ForEachRaw( auto&& F );

    /*
     *
     * Force using shared_ptr type
     *
     * */
    void
    ForEachShared( auto&& F );


    /*
     *
     * Using Container type
     *
     * */
    void
    ForEachOriginalIndex( auto&& F );

    /*
     *
     * Using raw-pointer type
     *
     * */
    void
    ForEachRawIndex( auto&& F );

    /*
     *
     * Force using shared_ptr type
     *
     * */
    void
    ForEachSharedIndex( auto&& F );

    bool
    NotEmpty( );

    void
    Clear( );

    uint64_t
    GetHash( ) const noexcept;

private:
    uint64_t m_CacheHash { };

    std::vector<Container> m_CachedConcepts { };

    friend class ConceptList;
};

#include "ConceptSetCache.inl"

template <typename Ty>
using ConceptSetCacheShared = ConceptSetCache<std::shared_ptr<Ty>>;

/*
 *
 * Use weak_ptr for storing, but access function still use shared_ptr
 *
 * */
template <typename Ty>
using ConceptSetCacheWeak = ConceptSetCache<std::weak_ptr<Ty>>;
