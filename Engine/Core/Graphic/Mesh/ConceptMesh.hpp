//
// Created by samsa on 10/23/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <span>

struct SubMeshSpan {
    std::string          SubMeshName { };
    std::span<glm::vec3> VertexRange;

    // Starting from 0 for every sub-mesh
    std::vector<uint32_t> RebasedIndexRange;

    SubMeshSpan( auto&& Name, auto&& VertexRange, auto&& RebasedIndex )
    {
        this->SubMeshName       = std::forward<decltype( Name )>( Name );
        this->VertexRange       = std::forward<decltype( VertexRange )>( VertexRange );
        this->RebasedIndexRange = std::forward<decltype( RebasedIndex )>( RebasedIndex );
    }
};

struct GLBufferHandle {
    uint32_t VAO { }, VBO { }, EBO { };

    ~GLBufferHandle( );

    ENABLE_IMGUI( GLBufferHandle )
};

enum MeshVertexFeature : uint32_t {
    eFeaturePosition = 1 << 1,
    eFeatureNormal   = 1 << 2,
    eFeatureUV0      = 1 << 3,
    eFeatureDefault  = eFeaturePosition | eFeatureNormal
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

    ConceptMesh( const std::string& MeshFilePath, const uint32_t Features = MeshVertexFeature::eFeatureDefault );

    const auto&
    GetVertices( ) const noexcept { return m_Vertices; }

    const auto&
    GetIndices( ) const noexcept { return m_Indices; }

    [[nodiscard]] auto&
    GetGLBufferHandle( ) noexcept { return m_GLBufferHandle; }

    [[nodiscard]] const auto&
    GetSubMeshes( ) const noexcept { return m_SubMeshes; }

    [[nodiscard]] const auto&
    GetFilePath( ) const noexcept { return m_FilePath; }

    void
    SetVertexFeature( const MeshVertexFeature Feature ) noexcept { m_VertexFeatures = Feature; }

protected:
    /*
     *
     * Vertex features
     *
     * */
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_UV0s;

    std::vector<uint32_t>    m_Indices;
    std::vector<SubMeshSpan> m_SubMeshes;

    MeshVertexFeature m_VertexFeatures = MeshVertexFeature::eFeatureDefault;

    std::string m_FilePath { };

    GLBufferHandle m_GLBufferHandle;

    friend class SerializerModel;

    ENABLE_IMGUI( ConceptMesh )
};
