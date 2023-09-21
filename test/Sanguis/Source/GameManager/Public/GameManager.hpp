//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

class GameManager : public ConceptApplicable
{
    DECLARE_CONCEPT( GameManager, ConceptApplicable )

public:
    GameManager( );

    void
    Apply( ) override;

private:
    std::unique_ptr<class SaEffect> m_Effect;

    std::shared_ptr<class ParticlePool> m_ParticlePool;

    ENABLE_IMGUI( GameManager )
};