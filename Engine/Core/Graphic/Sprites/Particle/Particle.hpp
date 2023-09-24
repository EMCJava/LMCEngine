//
// Created by samsa on 9/21/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class Particle
{

public:
    void
    SetLifeTime( FloatTy Lifetime );
    void
    AlterLifeTime( FloatTy DeltaLifetime );
    [[nodiscard]] bool
    IsAlive( ) const { return m_LifeTime > 0; }

    auto&
    GetOrientation( ) { return m_Orientation; }

    auto&
    GetColor( ) { return m_Color; }

    auto&
    GetVelocity( ) { return m_Velocity; }

    auto&
    GetAngularVelocity( ) { return m_AngularVelocity; }

    auto&
    GetLinearColorVelocity( ) { return m_LinearColorVelocity; }

private:
    FloatTy     m_LifeTime { 0 };
    Orientation m_Orientation { };
    glm::vec4   m_Color;

    FloatTy   m_AngularVelocity { };
    glm::vec4 m_LinearColorVelocity { };
    glm::vec3 m_Velocity { };
};
