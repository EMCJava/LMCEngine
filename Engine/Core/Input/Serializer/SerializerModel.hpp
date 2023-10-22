//
// Created by samsa on 10/22/2023.
//

#pragma once

#include <string>
#include <memory>

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
    SetFilePath( const std::string& FilePath ) { m_FilePath = FilePath; }

    bool
    LoadModel( );

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
