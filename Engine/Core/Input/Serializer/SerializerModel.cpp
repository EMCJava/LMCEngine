//
// Created by samsa on 10/22/2023.
//

#include "SerializerModel.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <assimp/Importer.hpp>    // C++ importer interface
#include <assimp/scene.h>         // Output data structure
#include <assimp/postprocess.h>   // Post processing flags

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <string>
#include <numeric>

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

void
FetchMesh( aiScene const* ModelScene, const aiNode* Node, const aiMatrix4x4& Transform, auto&& MeshProcessor )
{
    if ( Node->mMeshes != nullptr )
    {
        auto* Mesh = ModelScene->mMeshes[ *Node->mMeshes ];
        REQUIRED_IF( Mesh->mNumFaces > 0 && Mesh->mFaces[ 0 ].mNumIndices == 3 )
        {
            MeshProcessor( ModelScene, Mesh, Transform );
        }
    }

    for ( int i = 0; i < Node->mNumChildren; ++i )
    {
        const auto* CurrentNode = Node->mChildren[ i ];
        FetchMesh( ModelScene, CurrentNode, Transform * CurrentNode->mTransformation, MeshProcessor );
    }
}

bool
SerializerModel::ToMesh( ConceptMesh* ToMesh )
{
    if ( !m_ModelScene && !LoadModel( ) ) return false;

    ToMesh->m_FilePath = m_FilePath;
    ToMesh->m_Vertices_ColorPack.clear( );
    ToMesh->m_Normals.clear( );
    ToMesh->m_Indices.clear( );
    ToMesh->m_SubMeshes.clear( );

    ::FetchMesh( m_ModelScene, m_ModelScene->mRootNode, m_ModelScene->mRootNode->mTransformation, [ &ToMesh ]( aiScene const* ModelScene, aiMesh* Mesh, const aiMatrix4x4& Transform ) {
        static_assert( sizeof( glm::vec3 ) == sizeof( decltype( *Mesh->mVertices ) ) );

        /*
         *
         * Material setup
         *
         * */
        auto*     Material     = ModelScene->mMaterials[ Mesh->mMaterialIndex ];
        aiColor3D DiffuseColor = { 1, 0, 0.86 };
        REQUIRED( Material->Get( AI_MATKEY_COLOR_DIFFUSE, DiffuseColor ) == AI_SUCCESS )

        float ColorPack = PackColor( DiffuseColor );

        /*
         *
         * Texture setup
         *
         * */
        //        aiString DiffuseTexturePath;
        //        REQUIRED_IF( Material->Get( AI_MATKEY_TEXTURE_DIFFUSE( 0 ), DiffuseTexturePath ) == AI_SUCCESS )
        //        {
        //            spdlog::info( "> tex: {}", DiffuseTexturePath.C_Str( ) );
        //        }

        /*
         *
         * Vertex and normal buffer
         *
         * */
        const auto OldVerticesSize = ToMesh->m_Vertices_ColorPack.size( );
        ToMesh->m_Vertices_ColorPack.resize( OldVerticesSize + Mesh->mNumVertices );
        ToMesh->m_Normals.resize( OldVerticesSize + Mesh->mNumVertices );

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
        size_t NumberOfIndices = Mesh->mNumFaces * 3;

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

        // Keep submesh record
        {
            ToMesh->m_SubMeshes.emplace_back( Mesh->mName.C_Str( ),
                                              std::span<glm::vec4> { (glm::vec4*) nullptr, Mesh->mNumVertices },
                                              std::vector<uint32_t>( NumberOfIndices ) );

            // Need to recalculate the indices
            auto& RebasedIndexRange = ToMesh->m_SubMeshes.back( ).RebasedIndexRange;
            for ( int i = 0; i < NumberOfIndices; ++i )
                RebasedIndexRange[ i ] = ToMesh->m_Indices[ OldIndicesSize + i ] - OldVerticesSize;
        }
    } );

    {
        auto* BeginVer = ToMesh->m_Vertices_ColorPack.data( );

        for ( auto& SubMesh : ToMesh->m_SubMeshes )
        {
            SubMesh.VertexRange = { BeginVer, SubMesh.VertexRange.size( ) };
            BeginVer += SubMesh.VertexRange.size( );
        }

        REQUIRED( BeginVer == ToMesh->m_Vertices_ColorPack.data( ) + ToMesh->m_Vertices_ColorPack.size( ),
                  throw std::runtime_error( "SubMeshesRecord is not valid" ) )
    }

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    auto& GLBuffer = ToMesh->GetGLBufferHandle( );
    GL_CHECK( gl->GenVertexArrays( 1, &GLBuffer.VAO ) )
    GL_CHECK( gl->BindVertexArray( GLBuffer.VAO ) )

    // 2. copy our vertices array in a buffer for OpenGL to use
    GL_CHECK( gl->GenBuffers( 1, &GLBuffer.VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, GLBuffer.VBO ) )

    /*
     *
     * Data arrange {[Vertices...], [Normals...]}
     *
     * */
    // * 2 for normals

    std::vector<GLBufferRange> BufferSpans;
    BufferSpans.emplace_back( ToMesh->m_Vertices_ColorPack );
    BufferSpans.emplace_back( ToMesh->m_Normals );

    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER,
                              std::accumulate( BufferSpans.begin( ), BufferSpans.end( ), 0, []( int Size, const GLBufferRange& GLBR ) {
                                  return GLBR.Size + Size;
                              } ),
                              nullptr, GL_STATIC_DRAW ) )

    std::accumulate( BufferSpans.begin( ), BufferSpans.end( ), 0, [ gl, Index = 0 ]( size_t Offset, const GLBufferRange& GLBR ) mutable {
        GL_CHECK( gl->BufferSubData( GL_ARRAY_BUFFER, Offset, GLBR.Size, GLBR.Data ) )
        REQUIRED( GLBR.Stride % sizeof( float ) == 0, throw std::runtime_error( "Stride is not a multiple of 4" ) )
        GL_CHECK( gl->VertexAttribPointer( Index, GLBR.Stride / sizeof( float ), GL_FLOAT, GL_FALSE, GLBR.Stride, (void*) Offset ) )
        GL_CHECK( gl->EnableVertexAttribArray( Index++ ) )
        return Offset + GLBR.Size;
    } );

    GL_CHECK( gl->GenBuffers( 1, &GLBuffer.EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, GLBuffer.EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint32_t ) * ToMesh->m_Indices.size( ), ToMesh->m_Indices.data( ), GL_STATIC_DRAW ) )

    return true;
}

bool
SerializerModel::ToMesh( const std::string& FilePath, ConceptMesh* ToMesh )
{
    SerializerModel Tmp;
    Tmp.SetFilePath( FilePath );
    return Tmp.ToMesh( ToMesh );
}
