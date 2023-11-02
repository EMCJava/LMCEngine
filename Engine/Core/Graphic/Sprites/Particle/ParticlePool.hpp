//
// Created by samsa on 9/21/2023.
//

#pragma once

#include "Particle.hpp"

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <ranges>
#include <array>

// TODO: Might need to use Sprite instead for more general particle system
class ParticlePool : public ConceptApplicable
{
    DECLARE_CONCEPT( ParticlePool, ConceptApplicable );

    friend class ParticlePoolConceptRender;

    void
    ForEach( auto&& Func );

    void
    ForEachRange( auto&& Func, size_t Start, size_t End );

    /*
     *
     * Should be called by AddParticle after configuring the particle
     *
     * */
    void
    PushLastParticleInHeap( );

public:
    static constexpr size_t MAX_PARTICLES = 1000;

    ParticlePool( );

    void
    KillFirstToDieParticle( );

    /*
     *
     * Apply Configurator before pushing to min-heap
     * Return nullptr if reach max particle available
     *
     * */
    Particle*
    AddParticle( auto&& Configurator );

    void
    Apply( ) override;

    void
    SetSprite( const std::shared_ptr<class SpriteSquareTexture>& Sprite );

    size_t
    GetParticleCount( ) const;

protected:
    std::array<Particle, MAX_PARTICLES> m_MinHeapParticles;
    size_t                              m_ParticleCount = 0;

    std::shared_ptr<class SpriteSquareTexture> m_Sprite;

    ENABLE_IMGUI( ParticlePool )
};

Particle*
ParticlePool::AddParticle( auto&& Configurator )
{
    if ( m_ParticleCount >= m_MinHeapParticles.size( ) ) return nullptr;

    Configurator( m_MinHeapParticles[ m_ParticleCount ] );
    PushLastParticleInHeap( );
}