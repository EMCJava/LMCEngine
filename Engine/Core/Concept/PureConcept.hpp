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

protected:
    FastRandom               m_ConceptsStateHash { FastRandom::FromUint64( ConceptsStateHashInit.NextUint64( ) ) };
    inline static FastRandom ConceptsStateHashInit { FastRandom::FromRand( ) };

    /*
     *
     * ImGui interface fallback
     *
     * */
    static void ToImGuiWidgetInternal( const char* /*unused*/, PureConcept* /*unused*/ ) { }
};
