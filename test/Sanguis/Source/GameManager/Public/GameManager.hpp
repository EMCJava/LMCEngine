//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <glm/vec3.hpp>

class GameManager : public ConceptApplicable
{
    DECLARE_CONCEPT( GameManager, ConceptApplicable )

    void
    AddSlotHighlightUI( );

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
    std::shared_ptr<class Concept>           m_BaseSlotParticleParent;

    const glm::vec3 m_EditingAreaCoord = { 600, 30, 0 };
    const FloatTy   m_EditorAreaScale  = 0.7F;
    glm::vec3       m_BoardDimensions { };

    /*
     *
     * Game Logics
     *
     * */
    std::unique_ptr<class SaBaseBoard> m_BaseBoard;

    ENABLE_IMGUI( GameManager )
};