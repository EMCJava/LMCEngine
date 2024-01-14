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
    [[nodiscard]] FloatTy
    GetLifeTime( ) const { return m_LifeTime; }
    [[nodiscard]] bool
    IsAlive( ) const { return m_LifeTime > 0; }


    auto&
    GetOrientation( ) noexcept { return m_Orientation; }

    auto&
    GetColor( ) noexcept { return m_Color; }

    auto&
    GetVelocity( ) noexcept { return m_Velocity; }

    auto&
    GetAngularVelocity( ) noexcept { return m_AngularVelocity; }

    auto&
    GetLinearColorVelocity( ) noexcept { return m_LinearColorVelocity; }

    void
    Update( FloatTy DeltaTime );

private:
    FloatTy     m_LifeTime { 0 };
    Orientation m_Orientation { };
    glm::vec4   m_Color;

    FloatTy   m_AngularVelocity { };
    glm::vec4 m_LinearColorVelocity { };
    glm::vec3 m_Velocity { };
};

struct ParticleTimeGreater {
    bool operator( )( Particle& Left, Particle& Right ) const
        noexcept( noexcept( Left.GetLifeTime( ) > Right.GetLifeTime( ) ) )
    {
        return Left.GetLifeTime( ) > Right.GetLifeTime( );
    }
};
