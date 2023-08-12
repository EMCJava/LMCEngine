//
// Created by loyus on 7/9/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include "DynamicLibrary.hpp"

#include <map>

class DynamicConcept : public DynamicLibrary
{
    bool
    LoadFunctions( );

public:
    using DynamicLibrary::DynamicLibrary;

    DynamicConcept( const std::string& Path );
    ~DynamicConcept( );

    /*
     *
     * Load the dynamic library and obtain allocator + de-allocator
     * If Allocate == true, the allocator will be used to create new concept
     *
     * */
    bool
    Load( const std::string_view& Path, bool Allocate = true );

    /*
     *
     * Reload the dynamic library and obtain allocator + de-allocator
     * It will call DeAllocateConcept before reloading
     *
     * */
    bool
    Reload( bool ReAllocate = true );

    /*
     *
     * Allocate a new concept
     * It will call DeAllocateConcept before allocating
     *
     * */
    void
    AllocateConcept( );

    void
    DeAllocateConcept( );

    class Concept*
    operator->( )
    {
        return m_Concept;
    }

    template <typename Ty>
    Ty*
    As( )
    {
        return static_cast<Ty*>( m_Concept );
    }

    const auto& GetConceptToImGuiFuncPtrMap( ) { return m_ConceptToImGuiFuncPtrMap; }

private:
    void* ( *m_Allocator )( )                    = nullptr;
    void ( *m_DeAllocator )( void* )             = nullptr;
    NamingCollectionMap* m_ConceptNameCollection = nullptr;

    std::map<uint32_t, void ( * )( const char*, void* )> m_ConceptToImGuiFuncPtrMap { };

    class Concept* m_Concept = nullptr;
};
