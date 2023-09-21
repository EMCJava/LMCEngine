//
// Created by samsa on 9/21/2023.
//

#include "ParticleAttributesRandomizer.hpp"

#include <random>

ParticleAttributesRandomizer::ParticleAttributesRandomizer( )
{
    // Seed with a real random value, if available
    std::random_device r;

    // Generate a normal distribution
    std::seed_seq seed { r( ), r( ), r( ), r( ), r( ), r( ), r( ), r( ) };
    m_Engine = std::mt19937 { seed };
}

namespace
{
Vec3
RandVec( const auto& min, const auto max, auto& dis, auto& gen )
{
    glm::vec3 randomVec;
    randomVec.x = min.x + dis( gen ) * ( max.x - min.x );
    randomVec.y = min.y + dis( gen ) * ( max.y - min.y );
    randomVec.z = min.z + dis( gen ) * ( max.z - min.z );

    return Vec3 { randomVec.x, randomVec.y, randomVec.z };
}
}   // namespace

void
ParticleAttributesRandomizer::Apply( Particle& P )
{
    P.GetVelocity( )        = RandVec( m_VelocityMin, m_VelocityMax, m_NormalDist, m_Engine );
    P.GetAngularVelocity( ) = static_cast<FloatTy>( m_AngularVelocityMin + ( m_AngularVelocityMax - m_AngularVelocityMin ) * m_NormalDist( m_Engine ) );

    const auto Scale = m_LinearScaleMin + ( m_LinearScaleMax - m_LinearScaleMin ) * (float) m_NormalDist( m_Engine );
    P.GetOrientation( ).SetScale( Scale.x, Scale.y, Scale.z );
}

void
ParticleAttributesRandomizer::SetVelocity( const glm::vec3& min, const glm::vec3& max )
{
    m_VelocityMin = min;
    m_VelocityMax = max;
}

void
ParticleAttributesRandomizer::SetAngularVelocity( const FloatTy& min, const FloatTy& max )
{
    m_AngularVelocityMin = min;
    m_AngularVelocityMax = max;
}

void
ParticleAttributesRandomizer::SetLinearScale( const glm::vec3& min, const glm::vec3& max )
{
    m_LinearScaleMin = min;
    m_LinearScaleMax = max;
}
