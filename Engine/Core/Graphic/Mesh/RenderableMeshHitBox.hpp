//
// Created by samsa on 11/30/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>

#include "ConceptMesh.hpp"

class RenderableMeshHitBox : public ConceptRenderable
{
    DECLARE_CONCEPT( RenderableMeshHitBox, ConceptRenderable )

    void RenderFrame( );

public:
    RenderableMeshHitBox( std::vector<float> Vertices, std::vector<uint32_t> Indices );

    void
    Render( ) override;

private:
    uint32_t m_NumOfIndices = 0;

    GLBufferHandle m_BufferHandle;
};
