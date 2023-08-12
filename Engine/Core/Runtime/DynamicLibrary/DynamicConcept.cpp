//
// Created by loyus on 7/9/2023.
//

#include "DynamicConcept.hpp"

#include <Engine/Core/Concept/ConceptCore.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>

DynamicConcept::~DynamicConcept( )
{
    DeAllocateConcept( );

    delete m_ConceptNameCollection;
    m_ConceptNameCollection = nullptr;

    m_Concept     = nullptr;
    m_DeAllocator = nullptr;
    m_Allocator   = nullptr;
}

bool
DynamicConcept::Load( const std::string_view& Path, bool Allocate )
{
    bool result = DynamicLibrary::Load( Path );
    if ( !result )
    {
        return false;
    }

    if ( !LoadFunctions( ) )
    {
        return false;
    }

    if ( Allocate )
    {
        AllocateConcept( );
    }

    return true;
}

bool
DynamicConcept::Reload( bool ReAllocate )
{
    DeAllocateConcept( );

    bool result = DynamicLibrary::Reload( );
    if ( !result )
    {
        return false;
    }

    if ( !LoadFunctions( ) )
    {
        return false;
    }

    if ( ReAllocate )
    {
        AllocateConcept( );
    }

    return true;
}

bool
DynamicConcept::LoadFunctions( )
{
    LoadSymbolAs( "mem_alloc", m_Allocator );
    LoadSymbolAs( "mem_free", m_DeAllocator );

    m_ConceptToImGuiFuncPtrMap.clear( );
    NamingCollectionMap* ( *GetConceptNames )( ) = nullptr;
    LoadSymbolAs( "GetConceptNames", GetConceptNames );
    REQUIRED_IF( GetConceptNames != nullptr )
    {
        delete m_ConceptNameCollection;
        m_ConceptNameCollection = GetConceptNames( );

        const std::string ToImGuiWidgetPrefix = "ToImGuiWidget_";
        void ( *ToImGuiFunPtr )( const char*, void* );
        for ( size_t i = 0; i < m_ConceptNameCollection->ConceptLen; i++ )
        {
            ToImGuiFunPtr       = nullptr;
            std::string FunName = ToImGuiWidgetPrefix + std::string( m_ConceptNameCollection->Concepts[ i ].NamePre );
            LoadSymbolAs( FunName, ToImGuiFunPtr );

            if ( ToImGuiFunPtr != nullptr )
            {
                spdlog::info( "Found ToImGuiWidget function: {}", std::string( m_ConceptNameCollection->Concepts[ i ].NamePre ) );

                m_ConceptToImGuiFuncPtrMap[ m_ConceptNameCollection->Concepts[ i ].ID ] = ToImGuiFunPtr;
            }
        }
    }

    return m_Allocator != nullptr && m_DeAllocator != nullptr;
}

void
DynamicConcept::AllocateConcept( )
{
    DeAllocateConcept( );
    if ( m_Allocator != nullptr )
    {
        m_Concept = (class Concept*) m_Allocator( );
    }
}

void
DynamicConcept::DeAllocateConcept( )
{
    if ( m_Concept != nullptr )
    {
        REQUIRED( m_DeAllocator != nullptr );
        m_DeAllocator( m_Concept );
        m_Concept = nullptr;
    }
}
