//
// Created by samsa on 9/21/2023.
//

#pragma once

#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class Particle
{

public:
    void
    SetAlive( bool Alive = true );
    [[nodiscard]] bool
    IsAlive( ) const { return m_IsAlive; }

    Orientation&
    GetOrientation( ) { return m_Orientation; }

    OrientationCoordinate::Coordinate&
    GetVelocity( ) { return m_Velocity; }

    OrientationCoordinate::Coordinate&
    GetScale( ) { return m_Scale; }

    FloatTy&
    GetAngularVelocity( ) { return m_AngularVelocity; }

private:
    bool m_IsAlive { false };

    FloatTy                           m_AngularVelocity { };
    OrientationCoordinate::Coordinate m_Velocity { };
    OrientationCoordinate::Coordinate m_Scale { 1, 1, 1 };
    Orientation                       m_Orientation { };
};
