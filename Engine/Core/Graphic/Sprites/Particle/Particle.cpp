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
