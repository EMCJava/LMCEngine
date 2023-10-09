//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

    std::unique_ptr<class ParticleAttributesRandomizer> m_PAR;

    std::shared_ptr<class PureConceptCamera> m_Camera;
    std::shared_ptr<class Concept>           m_BaseSlotParticleParent;

    std::shared_ptr<class RectButton> m_UpdateSlotsButton;

    const glm::vec3 m_EditingAreaCoord = { 600, 30, 0 };
    const FloatTy   m_EditorAreaScale  = 0.7F;
    const FloatTy   m_SlotScale        = 0.45F;
    const FloatTy   m_IconScale        = m_SlotScale * 0.7F;
    glm::vec3       m_BoardDimensions { };

    const FloatTy m_ParticleScriptsize = 512;

    /*
     *
     * Game Logics
     *
     * */
    std::unique_ptr<class SaBaseBoard> m_BaseBoard;

    // Mouse Holding
    glm::vec2            m_MouseStartPosition  = { };
    glm::vec3            m_SpriteStartPosition = { };
    struct SpriteHitBox* m_MenuHoldingSprite   = { };

    // Menu
    std::vector<struct SpriteHitBox> m_MenuItems;
    std::vector<struct SpriteHitBox> m_BaseSlots;

    ENABLE_IMGUI( GameManager )
};

struct NodeSpritePair {
    std::shared_ptr<class SpriteSquareTexture> Sprite;
    std::shared_ptr<class SaControlNode>       Node;

    std::string NodeName = "Empty";

    void Swap( NodeSpritePair& Other, bool SwapCoordinate = true );

    ENABLE_IMGUI( NodeSpritePair )
};

struct SpriteHitBox {
    NodeSpritePair                               Data;
    std::shared_ptr<class PureConceptAABBSquare> HitBox;

    std::string SlotName = "Nan";

    void Swap( SpriteHitBox& Other, bool SwapCoordinate = true );

    ENABLE_IMGUI( SpriteHitBox )
};