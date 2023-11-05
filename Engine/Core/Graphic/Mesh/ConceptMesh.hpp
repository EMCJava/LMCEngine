//
// Created by samsa on 10/23/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

class ConceptMesh : public ConceptRenderable
    , public Orientation
{
    DECLARE_CONCEPT( ConceptMesh, ConceptRenderable )

public:
    void
    Render( ) override;

protected:
    std::vector<glm::vec4> m_Vertices_ColorPack;
    std::vector<glm::vec3> m_Normals;
    std::vector<uint32_t>  m_Indices;

    uint32_t m_VAO { }, m_VBO { }, m_EBO { };

    friend class SerializerModel;

    ENABLE_IMGUI( ConceptMesh )
};
