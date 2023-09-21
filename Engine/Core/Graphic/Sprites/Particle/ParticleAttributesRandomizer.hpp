//
// Created by samsa on 9/21/2023.
//

#pragma once

#include "Particle.hpp"

// (っ˘̩╭╮˘̩)っ
#include <glm/glm.hpp>

#include <random>

class ParticleAttributesRandomizer
{
public:
    ParticleAttributesRandomizer( );

    void
    Apply( Particle& P );

    void
    SetVelocity( const glm::vec3& min, const glm::vec3& max );

    void
    SetAngularVelocity( const FloatTy& min, const FloatTy& max );

private:
    glm::vec3 m_VelocityMin { }, m_VelocityMax { };
    FloatTy   m_AngularVelocityMin { }, m_AngularVelocityMax { };

    std::mt19937                     m_Engine;
    std::uniform_real_distribution<> m_NormalDist;
};
