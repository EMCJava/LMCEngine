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

    void
    TestInvokable( );

private:
    std::unique_ptr<class SaEffect>                  m_Effect;
    std::vector<std::shared_ptr<class ParticlePool>> m_ParticlePools;

    std::map<std::string, std::shared_ptr<class PureConceptAABBSquare>> m_BaseSlotHitBoxes;

    std::unique_ptr<class ParticleAttributesRandomizer> m_PAR;

    std::shared_ptr<class PureConceptCamera> m_Camera;

    ENABLE_IMGUI( GameManager )
};