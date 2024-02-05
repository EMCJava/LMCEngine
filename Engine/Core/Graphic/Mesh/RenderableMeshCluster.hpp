//
// Created by samsa on 2/2/2024.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>

class RenderableMeshCluster : public ConceptRenderable
{
    DECLARE_CONCEPT( RenderableMeshCluster, ConceptRenderable )

public:
    RenderableMeshCluster( );

    void
    Render( ) override;

protected:
    ConceptSetCache<std::shared_ptr<class RenderableMesh>> m_RenderableMeshes;

    ENABLE_IMGUI( RenderableMeshCluster )
};
