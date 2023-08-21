//
// Created by loyus on 8/1/2023.
//

#include "PureConcept.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( PureConcept )
DEFINE_SIMPLE_IMGUI_TYPE( PureConcept, m_ConceptsStateHash )

uint64_t
PureConcept::GetHash( ) const noexcept
{
    return m_ConceptsStateHash.SeekUint64( );
}
