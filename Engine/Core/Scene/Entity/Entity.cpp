//
// Created by samsa on 12/15/2023.
//

#include "Entity.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <ImGui/ImGui.hpp>

DEFINE_CONCEPT_DS( Entity )
DEFINE_SIMPLE_IMGUI_TYPE( Entity )

void
Entity::UpdateOrientation( const Orientation& Ori )
{
    AlterOrientation( Ori - m_Orientation );
}

void
Entity::AlterOrientation( const Orientation& Ori )
{
    m_Orientation += Ori;

    std::vector<std::shared_ptr<Entity>> SubEntity;
    GetConcepts( SubEntity, false );

    for ( auto& Sub : SubEntity )
        Sub->AlterOrientation( Ori );
}

void
Entity::UpdateGlobalOrientation( const Orientation& Ori )
{
    UpdateOrientation( Ori );

    if ( auto ParentEntity = GetOwner( )->TryCast<Entity>( ); ParentEntity != nullptr )
    {
        ParentEntity->m_Orientation;
    }
}
