//
// Created by samsa on 9/21/2023.
//

#pragma once

#include "Particle.hpp"

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <array>

// TODO: Might need to use Sprite instead for more general particle system
class ParticlePool : public ConceptApplicable
{
    DECLARE_CONCEPT( ParticlePool, ConceptApplicable );

    friend class ParticlePoolConceptRender;

    void
    ForEach( auto&& Func );

public:
    static constexpr size_t MAX_PARTICLES = 80000;

    ParticlePool( );

    void
    RemoveFirstParticle( );

    Particle&
    AddParticle( );

    void
    Apply( ) override;

    void
    SetSprite( const std::shared_ptr<class SpriteSquareTexture>& Sprite );

protected:
    /*
     *
     * Available slot will be MAX_PARTICLES - 1 to save my time from finding edge cases
     *
     * */
    std::array<Particle, MAX_PARTICLES> m_Particles;

    size_t m_StartIndex = 0, m_EndIndex = 0;

    std::shared_ptr<class SpriteSquareTexture> m_Sprite;

    ENABLE_IMGUI( ParticlePool )
};
