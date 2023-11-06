//
// Created by samsa on 10/22/2023.
//

#include "SerializerModel.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <assimp/Importer.hpp>    // C++ importer interface
#include <assimp/scene.h>         // Output data structure
#include <assimp/postprocess.h>   // Post processing flags

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <string>

SerializerModel::SerializerModel( )  = default;
SerializerModel::~SerializerModel( ) = default;

SerializerModel::SerializerModel( const std::string& FilePath )
{
    SetFilePath( FilePath );
}

bool
SerializerModel::LoadModel( )
{
    // Create an instance of the Importer class
    m_ImporterContext = std::make_unique<Assimp::Importer>( );

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const auto LoadFlags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_GenNormals | aiProcess_SortByPType | aiProcess_GenBoundingBoxes;
    m_ModelScene         = m_ImporterContext->ReadFile( m_FilePath, LoadFlags );

    // If the import failed, report it
    if ( m_ModelScene == nullptr )
    {
        spdlog::error( "Failed loading model -{}-: {}", m_FilePath, m_ImporterContext->GetErrorString( ) );
        return false;
    }

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

FloatTy
PackColor( aiColor3D color )
{
    return round( color.r * 255.0 )
        + round( color.g * 255.0 ) * 256.0
        + round( color.b * 255.0 ) * 256.0 * 256.0;
}

bool
SerializerModel::LoadModel( ConceptMesh* ToMesh )
{
    if ( !m_ModelScene && !LoadModel( ) ) return false;

    ToMesh->m_Vertices_ColorPack.clear( );
    ToMesh->m_Normals.clear( );
    ToMesh->m_Indices.clear( );

    std::function<void( int, const aiNode*, const aiMatrix4x4& )> FetchMesh;
    FetchMesh = [ this, &FetchMesh, ToMesh ]( int Indent, const aiNode* Node, const aiMatrix4x4& Transform ) {
        if ( Node->mMeshes != nullptr )
        {
            spdlog::info( "{}Node: {}[{}]", std::string( Indent, '-' ), Node->mName.C_Str( ), *Node->mMeshes );
            auto* Mesh = m_ModelScene->mMeshes[ *Node->mMeshes ];

            REQUIRED_IF( Mesh->mNumFaces > 0 && Mesh->mFaces[ 0 ].mNumIndices == 3 )
            {
                static_assert( sizeof( glm::vec3 ) == sizeof( decltype( *Mesh->mVertices ) ) );

                /*
                 *
                 * Material setup
                 *
                 * */
                auto*     Material     = m_ModelScene->mMaterials[ Mesh->mMaterialIndex ];
                aiColor3D DiffuseColor = { 1, 0, 0.86 };
                REQUIRED_IF( Material->Get( AI_MATKEY_COLOR_DIFFUSE, DiffuseColor ) == AI_SUCCESS )
                {
                    spdlog::info( "{}> {},{},{}", std::string( Indent, '-' ), DiffuseColor.r, DiffuseColor.g, DiffuseColor.b );
                }

                float ColorPack = PackColor( DiffuseColor );

                /*
                 *
                 * Texture setup
                 *
                 * */
                aiString DiffuseTexturePath;
                REQUIRED_IF( Material->Get( AI_MATKEY_TEXTURE_DIFFUSE( 0 ), DiffuseTexturePath ) == AI_SUCCESS )
                {
                    spdlog::info( "{}> tex: {}", std::string( Indent, '-' ), DiffuseTexturePath.C_Str( ) );
                }

                /*
                 *
                 * Vertex and normal buffer
                 *
                 * */
                const auto OldVerticesSize = ToMesh->m_Vertices_ColorPack.size( );
                ToMesh->m_Vertices_ColorPack.resize( OldVerticesSize + Mesh->mNumVertices );
                ToMesh->m_Normals.resize( OldVerticesSize + Mesh->mNumVertices );

                // memcpy( ToMesh->m_Vertices.data( ) + OldVerticesSize, Mesh->mVertices, sizeof( glm::vec3 ) * Mesh->mNumVertices );
                for ( int i = 0; i < Mesh->mNumVertices; ++i )
                {
                    // Little hack
                    *(aiVector3D*) ( &ToMesh->m_Vertices_ColorPack[ OldVerticesSize + i ] ) = Transform * Mesh->mVertices[ i ];
                    ToMesh->m_Vertices_ColorPack[ OldVerticesSize + i ].w                   = ColorPack;
                }
                memcpy( ToMesh->m_Normals.data( ) + OldVerticesSize, Mesh->mNormals, sizeof( glm::vec3 ) * Mesh->mNumVertices );

                /*
                 *
                 * Base on the assumption that mesh only contains triangles
                 *
                 * */
                int NumberOfIndices = Mesh->mNumFaces * 3;

                /*
                 *
                 * Indices buffer
                 *
                 * */
                const auto OldIndicesSize = ToMesh->m_Indices.size( );
                ToMesh->m_Indices.resize( ToMesh->m_Indices.size( ) + NumberOfIndices );
                uint32_t* IndicesData = ToMesh->m_Indices.data( ) + OldIndicesSize;

                for ( int j = 0; j < Mesh->mNumFaces; ++j )
                {
                    for ( int k = 0; k < 3 /* For optimize */; ++k )
                    {
                        IndicesData[ k ] = Mesh->mFaces[ j ].mIndices[ k ] + OldVerticesSize;
                    }

                    IndicesData += 3;
                }

                spdlog::info( "{}>{}", std::string( Indent, '-' ), NumberOfIndices );
            }
        } else
        {
            spdlog::info( "{}Node: {}", std::string( Indent, '-' ), Node->mName.C_Str( ) );
        }

        for ( int i = 0; i < Node->mNumChildren; ++i )
        {
            const auto* CurrentNode = Node->mChildren[ i ];
            FetchMesh( Indent + 1, CurrentNode, Transform * CurrentNode->mTransformation );
        }
    };

    FetchMesh( 1, m_ModelScene->mRootNode, m_ModelScene->mRootNode->mTransformation );

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GL_CHECK( gl->GenVertexArrays( 1, &ToMesh->m_VAO ) )
    GL_CHECK( gl->BindVertexArray( ToMesh->m_VAO ) )

    // 2. copy our vertices array in a buffer for OpenGL to use
    GL_CHECK( gl->GenBuffers( 1, &ToMesh->m_VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, ToMesh->m_VBO ) )

    /*
     *
     * Data arrange {[Vertices...], [Normals...]}
     *
     * */
    // * 2 for normals
    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( glm::vec4 ) * ToMesh->m_Vertices_ColorPack.size( ) + sizeof( glm::vec3 ) * ToMesh->m_Normals.size( ), nullptr, GL_STATIC_DRAW ) )
    GL_CHECK( gl->BufferSubData( GL_ARRAY_BUFFER, 0,
                                 sizeof( glm::vec4 ) * ToMesh->m_Vertices_ColorPack.size( ), /* Vertices size */
                                 ToMesh->m_Vertices_ColorPack.data( ) ) );
    GL_CHECK( gl->BufferSubData( GL_ARRAY_BUFFER,
                                 sizeof( glm::vec4 ) * ToMesh->m_Vertices_ColorPack.size( ), /* Vertices size */
                                 sizeof( glm::vec3 ) * ToMesh->m_Normals.size( ),            /* Normals size */
                                 ToMesh->m_Normals.data( ) ) )

    // 3. then set our vertex attributes pointers
    GL_CHECK( gl->VertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), nullptr ) )
    GL_CHECK( gl->EnableVertexAttribArray( 0 ) )
    GL_CHECK( gl->VertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*) ( sizeof( glm::vec4 ) * ToMesh->m_Vertices_ColorPack.size( ) ) /* Vertices size */ ) )
    GL_CHECK( gl->EnableVertexAttribArray( 1 ) )

    GL_CHECK( gl->GenBuffers( 1, &ToMesh->m_EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, ToMesh->m_EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint32_t ) * ToMesh->m_Indices.size( ), ToMesh->m_Indices.data( ), GL_STATIC_DRAW ) )

    return true;
}
