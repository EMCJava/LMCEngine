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

    void
    SetLifetime( const FloatTy& min, const FloatTy& max );

    void
    SetLinearScale( const glm::vec3& min, const glm::vec3& max );

    void
    SetStartLinearColor( const glm::vec4& min, const glm::vec4& max );

    void
    SetEndLinearColor( const glm::vec4& min, const glm::vec4& max );

private:
    glm::vec3 m_VelocityMin { }, m_VelocityMax { };
    glm::vec3 m_LinearScaleMin { 1 }, m_LinearScaleMax { 1 };
    glm::vec4 m_StartLinearColorMin { 1 }, m_StartLinearColorMax { 1 };
    glm::vec4 m_EndLinearColorMin { 1 }, m_EndLinearColorMax { 1 };
    FloatTy   m_AngularVelocityMin { }, m_AngularVelocityMax { };
    FloatTy   m_LifetimeMin { 1 }, m_LifetimeMax { 1 };

    std::mt19937                     m_Engine;
    std::uniform_real_distribution<> m_NormalDist;
};
