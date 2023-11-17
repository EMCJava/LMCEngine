//
// Created by samsa on 10/23/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <span>

struct SubMeshSpan {
    std::string          SubMeshName { };
    std::span<glm::vec4> VertexRange;

    // Starting from 0 for every sub-mesh
    std::vector<uint32_t> RebasedIndexRange;
};

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

    [[nodiscard]] auto&
    GetSubMeshes( ) { return m_SubMeshes; }

    [[nodiscard]] auto&
    GetFilePath( ) { return m_FilePath; }

protected:
    std::vector<glm::vec4>   m_Vertices_ColorPack;
    std::vector<glm::vec3>   m_Normals;
    std::vector<uint32_t>    m_Indices;
    std::vector<SubMeshSpan> m_SubMeshes;

    std::string m_FilePath { };

    GLBufferHandle m_GLBufferHandle;

    friend class SerializerModel;

    ENABLE_IMGUI( ConceptMesh )
};
