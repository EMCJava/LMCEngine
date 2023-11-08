//
// Created by samsa on 10/22/2023.
//

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <span>

#include <glm/vec4.hpp>

// Forward declarations for Assimp library
namespace Assimp
{
class Importer;
}

struct SubMeshSpan {
    std::span<glm::vec4> VertexRange;
    std::span<uint32_t>  IndexRange;
};

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

    bool
    LoadModel( );

    bool
    ToMesh( class ConceptMesh* ToMesh, std::vector<SubMeshSpan>* SubMeshesRecord = nullptr );

    struct aiScene const*
    GetScene( ) const { return m_ModelScene; }

protected:
    std::string m_FilePath;

    std::unique_ptr<Assimp::Importer> m_ImporterContext;
    /*
     *
     * Manage by Assimp, release when context is destroyed
     *
     * */
    struct aiScene const* m_ModelScene { };
};
