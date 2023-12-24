//
// Created by samsa on 11/18/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class RenderableMesh : public ConceptRenderable
{
    DECLARE_CONCEPT( RenderableMesh, ConceptRenderable )

public:
    RenderableMesh( std::shared_ptr<class ConceptMesh> Mesh );

    void
    Render( ) override;

    void
    SetStaticMesh( std::shared_ptr<class ConceptMesh> Mesh );

    void
    SetMaterial( std::shared_ptr<class Material> Mat );

protected:
    std::shared_ptr<class ConceptMesh> m_ConceptMesh;
    std::shared_ptr<class Material>    m_Material;

    ENABLE_IMGUI( RenderableMesh )
};
