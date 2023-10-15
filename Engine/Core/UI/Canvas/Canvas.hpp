//
// Created by samsa on 10/12/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <memory>

class Canvas : public ConceptRenderable
{
    DECLARE_CONCEPT( Canvas, ConceptRenderable )

public:
    Canvas( );

    // Update cameras in sub-concepts
    void
    Render( ) override;

    void
    SetCanvasCamera( const std::shared_ptr<class PureConceptCamera>& Camera );

protected:
    std::shared_ptr<class PureConceptCamera> m_CanvasCamera;

    ENABLE_IMGUI( Canvas )
};
