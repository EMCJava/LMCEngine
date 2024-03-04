//
// Created by samsa on 10/22/2023.
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <span>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// Forward declarations for Assimp library
namespace Assimp
{
class Importer;
}

class SerializerModel
{
public:
    explicit SerializerModel( const std::string& FilePath );

    // Needed for std::unique_ptr<Assimp::Importer>
    SerializerModel( );
    ~SerializerModel( );

    void
    SetFilePath( const std::string& FilePath )
    {
        m_FilePath   = FilePath;
        m_ModelScene = nullptr;
    }

    void
    SetGlobalTransform( const glm::mat4& GlobalTransform )
    {
        m_GlobalTransform = GlobalTransform;
    }

    bool
    LoadModel( );

    bool
    ToMesh( class ConceptMesh* ToMesh );

    bool
    ToMeshCluster( class RenderableMeshCluster* ToMesh, uint32_t VertexFeature );

    static bool
    ToMesh( const std::string& FilePath, class ConceptMesh* ToMesh, const glm::mat4& Transform = glm::mat4( 1.0f ) );

    static std::shared_ptr<class ConceptMesh>
    ToMesh( const std::string& FilePath, const glm::mat4& Transform = glm::mat4( 1.0f ) );

    /*
     *
     * Combine Mesh Cluster
     *
     * */
    static bool
    ToMesh( const std::shared_ptr<class RenderableMeshCluster>& MeshCluster, class ConceptMesh* ToMesh );
    static std::shared_ptr<class ConceptMesh>
    ToMesh( const std::shared_ptr<class RenderableMeshCluster>& MeshCluster );

    static bool
    ToMeshCluster( const std::string& FilePath, class RenderableMeshCluster* ToMesh, uint32_t VertexFeature );

    static std::shared_ptr<class RenderableMeshCluster>
    ToMeshCluster( const std::string& FilePath, uint32_t VertexFeature );

    struct aiScene const*
    GetScene( ) const { return m_ModelScene; }

protected:
    std::string m_FilePath;

    glm::mat4 m_GlobalTransform { 1 };

    std::unique_ptr<Assimp::Importer> m_ImporterContext;
    /*
     *
     * Manage by Assimp, release when context is destroyed
     *
     * */
    struct aiScene const* m_ModelScene { };
};
