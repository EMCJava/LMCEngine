//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "ConceptCore.hpp"

#include <Engine/Core/Math/Random/FastRandom.hpp>

#pragma warning( Using dynamic_pointer_cast, performance can be impacted )
#define ConceptCasting std::dynamic_pointer_cast

class PureConcept
{
    DECLARE_CONCEPT( PureConcept )

public:
    /*
     *
     * Should be called after concept construction, use for construction time virtual functions call
     * Done automatically by AddConcept
     *
     * */
    void
    ConceptLateInitialize( ) { }

    uint64_t
    GetHash( ) const noexcept;

    /*
     *
     * Used to remove reference from owner
     *
     * */
    void
    Destroy( );

    /*
     *
     * if BelongsTo != nullptr, move this sub-concept as the first element in list (m_SubConcepts)
     * TODO: Consider moving this to Concept, as it need access to m_SubConcepts
     *
     * */
    void
    MoveToFirstAsSubConcept( );

    /*
     *
     * if BelongsTo != nullptr, move this sub-concept as the first element in list (m_SubConcepts)
     * TODO: Consider moving this to Concept, as it need access to m_SubConcepts
     *
     * */
    void
    MoveToLastAsSubConcept( );

    class Concept*
    GetOwner( ) const noexcept { return m_BelongsTo; }

    void
    DetachFromOwner( );

    virtual void
    SetEnabled( bool enabled ) { m_Enabled = enabled; }

    [[nodiscard]] inline bool
    IsEnabled( ) { return m_Enabled; }

    template <class ConceptType, typename... Args>
    static std::shared_ptr<ConceptType> CreateConcept( Args&&... params );

protected:
    FastRandom               m_ConceptsStateHash { FastRandom::FromUint64( ConceptsStateHashInit.NextUint64( ) ) };
    inline static FastRandom ConceptsStateHashInit { FastRandom::FromRand( ) };

    /*
     *
     * Overall switch to the concept, should check before doing anything with the concept
     *
     * */
    bool m_Enabled = true;

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

template <class ConceptType, typename... Args>
std::shared_ptr<ConceptType>
PureConcept::CreateConcept( Args&&... params )
{
    static_assert( ConceptType::template CanCastS<PureConcept>( ) );

    auto Result = ConceptCasting<ConceptType>( std::make_shared<ConceptType>( std::forward<Args>( params )... ) );
    Result->SetRuntimeName( Result->GetClassName( ) );
    Result->ConceptType::ConceptLateInitialize( );

    return Result;
}