//
// Created by loyus on 8/1/2023.
//

#include "PureConcept.hpp"

#include <Engine/Core/Concept/Concept.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT( PureConcept )
DEFINE_SIMPLE_IMGUI_TYPE( PureConcept, m_Enabled, m_ConceptsStateHash )

PureConcept::~PureConcept( )
{
    Destroy( );

    spdlog::trace( "{}::~{} -> {}", "PureConcept", "PureConcept", fmt::ptr( this ) );
}

uint64_t
PureConcept::GetHash( ) const noexcept
{
    return m_ConceptsStateHash.GetSeekValue( );
}

void
PureConcept::Destroy( )
{
    if ( m_BelongsTo != nullptr )
    {
        m_BelongsTo->RemoveConcept( this );
        m_BelongsTo = nullptr;
    }
}