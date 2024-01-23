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
    Orientation Diff;

    Diff.Coordinate = Ori.Coordinate - m_Orientation.Coordinate;
    Diff.Scale      = Ori.Scale - m_Orientation.Scale;

    if ( m_Orientation.IsUsingQuatRotation( ) )
    {
        Diff.SetQuat( m_Orientation.GetQuat( ) );
    } else
    {
        const auto& Vec4 = m_Orientation.GetQuatVec4( );
        Diff.SetRotation( Vec4.x, Vec4.y, Vec4.z );
        Diff.ActivateQuatRotation( );
    }

    Diff.SetQuat( m_Orientation.GetQuat( ) * glm::inverse( Diff.GetQuat( ) ) );

    AlterOrientation( Diff );
}

void
Entity::AlterOrientation( const Orientation& Ori )
{

    m_Orientation.Coordinate += Ori.Coordinate;
    m_Orientation.Scale += Ori.Scale;
    m_Orientation.ActivateQuatRotation( );
    m_Orientation.SetQuat( Ori.GetQuat( ) * m_Orientation.GetQuat( ) );

    std::vector<std::shared_ptr<Entity>> SubEntity;
    GetConcepts( SubEntity, false );

    for ( auto& Sub : SubEntity )
        Sub->AlterOrientation( Ori );
}
