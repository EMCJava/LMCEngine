//
// Created by samsa on 9/21/2023.
//

#include "Particle.hpp"

void
Particle::SetLifeTime( FloatTy Lifetime )
{
    m_LifeTime = Lifetime;
}

void
Particle::AlterLifeTime( FloatTy DeltaLifetime )
{
    m_LifeTime += DeltaLifetime;
}

void
Particle::Update( FloatTy DeltaTime )
{
    auto& Ori = GetOrientation( );
    Ori.Coordinate += GetVelocity( ) * DeltaTime;
    Ori.AlterRotation( 0, 0, GetAngularVelocity( ) * DeltaTime );

    GetColor( ) += GetLinearColorVelocity( ) * DeltaTime;
    AlterLifeTime( -DeltaTime );
}
