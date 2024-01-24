//
// Created by samsa on 10/24/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class WandEditorScene : public ConceptList
{
    DECLARE_CONCEPT( WandEditorScene, ConceptList )

    void
    AddSlotHighlightUI( );

public:
    WandEditorScene( );

    void
    Apply( ) override;

private:
    std::shared_ptr<class PureConceptOrthoCamera> m_UICamera;

    /*
     *
     * UI settings
     *
     * */
    const glm::vec3 m_EditingAreaCoord = { 600, 30, 0 };
    const FloatTy   m_EditorAreaScale  = 0.7F;
    const FloatTy   m_SlotScale        = 0.45F;
    const FloatTy   m_IconScale        = m_SlotScale * 0.7F;
    glm::vec3       m_BoardDimensions { };

    const FloatTy m_ParticleScriptsize = 512;

    /*
     *
     * UIs
     *
     * */
    std::shared_ptr<class RectButton> m_UpdateSlotsButton;

    /*
     *
     * UI Controls
     *
     * */
    std::vector<struct SpriteHitBox> m_MenuItems;
    std::vector<struct SpriteHitBox> m_BaseSlots;

    /*
     *
     * Mouse Holding
     *
     * */
    glm::vec2            m_MouseStartPosition  = { };
    glm::vec3            m_SpriteStartPosition = { };
    struct SpriteHitBox* m_MenuHoldingSprite   = { };

    /*
     *
     * Canvas
     *
     * */
    std::shared_ptr<class Canvas> m_UICanvas;
    std::shared_ptr<class Canvas> m_ParticleCanvas;
    std::shared_ptr<class Canvas> m_BaseSlotMenuCanvas;
    std::shared_ptr<class Canvas> m_WandEditorCanvas;

    /*
     *
     * Particles
     *
     * */
    std::vector<std::shared_ptr<class ParticlePool>> m_ParticlePools;

    /*
     *
     * Game logic
     *
     * */
    std::unique_ptr<class SaEffect>    m_Effect;
    std::unique_ptr<class SaBaseBoard> m_BaseBoard;

    std::unique_ptr<class ParticleAttributesRandomizer> m_PAR;

    ENABLE_IMGUI( WandEditorScene )
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