//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "ConceptCore.hpp"

#include <Engine/Core/Math/Random/FastRandom.hpp>

class PureConcept
{
    DECLARE_CONCEPT( PureConcept )

public:
    uint64_t
    GetHash( ) const noexcept;

    /*
     *
     * Used to remove reference from owner
     *
     * */
    void
    Destroy( );

protected:
    FastRandom               m_ConceptsStateHash { FastRandom::FromUint64( ConceptsStateHashInit.NextUint64( ) ) };
    inline static FastRandom ConceptsStateHashInit { FastRandom::FromRand( ) };

private:
    /*
     *
     * Pointer to who owns(calls AddConcept on) this concept
     * Useful in e.g. Destroy concept, PureConcept should not be able to delete itself
     *
     * */
    class Concept* m_BelongsTo = nullptr;

    friend class Concept;
    ENABLE_IMGUI( PureConcept )
};
