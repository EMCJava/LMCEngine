//
// Created by samsa on 8/31/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>
#include <Engine/Core/Scene/Orientation/OrientationCoordinate.hpp>

#include <glm/glm.hpp>

class RectButton : public ConceptApplicable
{
    DECLARE_CONCEPT( RectButton, ConceptApplicable )
public:
    RectButton( int Width, int Height );

    const OrientationCoordinate::Coordinate&
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    void
    SetActiveCamera( class PureConceptCamera* ActiveCamera );

    virtual void
    Apply( );

protected:
    std::shared_ptr<class SpriteSquare>       m_SpriteSquare;
    std::shared_ptr<class PureConceptAABBBox> m_HitBox;

    glm::vec4 m_DefaultColor { }, m_PressColor { };

    FloatTy m_PressReactTimeLeft = 0;
};
