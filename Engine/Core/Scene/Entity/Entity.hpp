//
// Created by samsa on 12/15/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>
#include <Engine/Core/Scene/Orientation/OrientationMatrix.hpp>

class Entity : public ConceptList
{
    DECLARE_CONCEPT( Entity, ConceptList )

public:
    Entity( )
    {
        SetSearchThrough( true );
    }

    /*
     *
     * Same as @UpdateOrientation but also alter the @m_RelToParent
     *
     * */
    void
    UpdateGlobalOrientation( const Orientation& Ori );

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
    OrientationMatrix m_Orientation;
    Orientation       m_RelToParent;

    ENABLE_IMGUI( Entity )
};
