//
// Created by loyus on 8/1/2023.
//

#include "PureConcept.hpp"

DEFINE_CONCEPT( PureConcept )

PureConcept::~PureConcept( )
{
    spdlog::critical( "!!!! PureConcept::~PureConcept(): {} !!!!", fmt::ptr( this ) );
}

uint64_t
PureConcept::GetHash( ) const noexcept
{
    return m_ConceptsStateHash.SeekUint64( );
}
