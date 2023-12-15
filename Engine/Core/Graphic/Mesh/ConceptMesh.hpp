//
// Created by samsa on 10/23/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

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

/*
 *
 * Can be shared
 *
 * */
class ConceptMesh : public ConceptList
{
    DECLARE_CONCEPT( ConceptMesh, ConceptList )

public:
    ConceptMesh( ) = default;

    ConceptMesh( const std::string& MeshFilePath );

    const auto&
    GetVerticesColorPack( ) const noexcept { return m_Vertices_ColorPack; }

    const auto&
    GetIndices( ) const noexcept { return m_Indices; }

    [[nodiscard]] auto&
    GetGLBufferHandle( ) noexcept { return m_GLBufferHandle; }

    [[nodiscard]] const auto&
    GetSubMeshes( ) const noexcept { return m_SubMeshes; }

    [[nodiscard]] const auto&
    GetFilePath( ) const noexcept { return m_FilePath; }

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
