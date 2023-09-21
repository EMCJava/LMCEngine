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

    FloatTy&
    GetAlpha( ) { return m_Alpha; }

    Vec3&
    GetVelocity( ) { return m_Velocity; }

    FloatTy&
    GetAngularVelocity( ) { return m_AngularVelocity; }

    FloatTy&
    GetAlphaVelocity( ) { return m_AlphaVelocity; }

private:
    bool m_IsAlive { false };

    FloatTy m_Alpha { 1 };

    FloatTy     m_AngularVelocity { };
    FloatTy     m_AlphaVelocity { };
    Vec3        m_Velocity { };
    Orientation m_Orientation { };
};
