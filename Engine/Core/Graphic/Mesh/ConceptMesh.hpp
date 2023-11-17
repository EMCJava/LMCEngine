//
// Created by samsa on 10/23/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct GLBufferHandle {
    uint32_t VAO { }, VBO { }, EBO { };
    ENABLE_IMGUI( GLBufferHandle )
};

class ConceptMesh : public PureConcept
{
    DECLARE_CONCEPT( ConceptMesh, PureConcept )

public:
    auto&
    GetVerticesColorPack( ) { return m_Vertices_ColorPack; }

    auto&
    GetIndices( ) { return m_Indices; }

    [[nodiscard]] auto&
    GetGLBufferHandle( ) { return m_GLBufferHandle; }

protected:
    std::vector<glm::vec4> m_Vertices_ColorPack;
    std::vector<glm::vec3> m_Normals;
    std::vector<uint32_t>  m_Indices;

    GLBufferHandle m_GLBufferHandle;

    friend class SerializerModel;

    ENABLE_IMGUI( ConceptMesh )
};
