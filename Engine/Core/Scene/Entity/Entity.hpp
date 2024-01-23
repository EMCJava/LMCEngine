//
// Created by samsa on 12/15/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class Entity : public ConceptList
{
    DECLARE_CONCEPT( Entity, ConceptList )

public:
    /*
     *
     * Set its orientation and update orientation of any Entity in sub-concept
     *
     * */
    void
    UpdateOrientation( const Orientation& Ori );

    /*
     *
     * Alter its orientation(**relatively**) and update orientation of any Entity in sub-concept
     *
     * */
    void
    AlterOrientation( const Orientation& Ori );

    const auto&
    GetOrientation( ) const noexcept { return m_Orientation; }

protected:
    Orientation m_Orientation;
    Orientation m_RelToParent;

    ENABLE_IMGUI( Entity )
};
