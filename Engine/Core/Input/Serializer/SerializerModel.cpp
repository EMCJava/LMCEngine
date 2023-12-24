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

struct GLBufferRange {
    const void* Data;
    size_t      Size;
    // Should be same as the size of the data
    // Buffers that type are the same will be put together
    size_t Stride;

    explicit GLBufferRange( const auto& Vec )
    {
        Data   = Vec.data( );
        Size   = Vec.size( ) * sizeof( *Vec.data( ) );
        Stride = sizeof( *Vec.data( ) );
    }
};

bool
SerializerModel::ToMesh( ConceptMesh* ToMesh )
{
    if ( !m_ModelScene && !LoadModel( ) ) return false;

    ToMesh->m_FilePath = m_FilePath;
    ToMesh->m_Vertices_ColorPack.clear( );
    ToMesh->m_Normals.clear( );
    ToMesh->m_Indices.clear( );
    ToMesh->m_SubMeshes.clear( );

    ::FetchMesh( m_ModelScene,
                 m_ModelScene->mRootNode,
                 m_ModelScene->mRootNode->mTransformation,
                 [ &ToMesh ]( aiScene const* ModelScene, aiMesh* Mesh, const aiMatrix4x4& Transform ) {
                     static_assert( sizeof( glm::vec3 ) == sizeof( decltype( *Mesh->mVertices ) ) );

                     /*
                      *
                      * Material setup
                      *
                      * */
                     auto*     Material         = ModelScene->mMaterials[ Mesh->mMaterialIndex ];
                     aiColor3D DiffuseColor     = { 1, 0, 0.86 };
                     aiReturn  ColorFetchResult = Material->Get( AI_MATKEY_COLOR_DIFFUSE, DiffuseColor );
                     if ( ColorFetchResult != AI_SUCCESS ) spdlog::warn( "No diffuse color defined in material:{}", Material->GetName( ).C_Str( ) );

                     float ColorPack = PackColor( DiffuseColor );

                     /*
                      *
                      * Texture setup
                      *
                      * */
                     //                     std::string Keys;
                     //                     for ( int np = 0; np < Material->mNumProperties; ++np )
                     //                     {
                     //                         Keys += Material->mProperties[ np ]->mKey.C_Str( );
                     //                         Keys += " ";
                     //                     }
                     //                     spdlog::info( "Texture keys: {}", Keys );

                     aiString DiffuseTexturePath;
                     if ( Material->Get( AI_MATKEY_TEXTURE_DIFFUSE( 0 ), DiffuseTexturePath ) == AI_SUCCESS )
                     {
                         spdlog::warn( "> Ignoring texture: {}", DiffuseTexturePath.C_Str( ) );
                     }

                     /*
                      *
                      * Vertex
                      *
                      * */
                     REQUIRED( ToMesh->m_VertexFeatures & eFeaturePosition, throw std::runtime_error( "Vertex position is required" ) );
                     const auto OldVerticesSize = ToMesh->m_Vertices_ColorPack.size( );
                     {
                         ToMesh->m_Vertices_ColorPack.resize( OldVerticesSize + Mesh->mNumVertices );

                         for ( int i = 0; i < Mesh->mNumVertices; ++i )
                         {
                             // Little hack
                             *(aiVector3D*) ( &ToMesh->m_Vertices_ColorPack[ OldVerticesSize + i ] ) = Transform * Mesh->mVertices[ i ];
                             ToMesh->m_Vertices_ColorPack[ OldVerticesSize + i ].w                   = ColorPack;
                         }
                     }

                     /*
                      *
                      *  Normal buffer
                      *
                      * */
                     if ( ToMesh->m_VertexFeatures & eFeatureNormal )
                     {
                         auto OldNormalsSize = ToMesh->m_Normals.size( );
                         ToMesh->m_Normals.resize( OldVerticesSize + Mesh->mNumVertices );
                         memcpy( ToMesh->m_Normals.data( ) + OldVerticesSize, Mesh->mNormals, sizeof( glm::vec3 ) * Mesh->mNumVertices );
                     }

                     /*
                      *
                      *  UV buffer
                      *
                      * */
                     if ( ToMesh->m_VertexFeatures & eFeatureUV0 )
                     {
                         const auto TextureIndex = 0;
                         if ( Mesh->mTextureCoords[ TextureIndex ] != nullptr )
                         {
                             REQUIRED_IF( Mesh->mNumUVComponents[ TextureIndex ] == 2 )   // glm::vec2
                             {
                                 const auto OldUVsSize = ToMesh->m_UV0s.size( );
                                 ToMesh->m_UV0s.resize( OldUVsSize + Mesh->mNumVertices );
                                 for ( int i = 0; i < Mesh->mNumVertices; ++i )
                                     ToMesh->m_UV0s[ OldUVsSize + i ] = *(glm::vec2*) &Mesh->mTextureCoords[ TextureIndex ][ i ];
                                 goto FeatureUV0_Found;
                             }
                         }

                         ToMesh->m_UV0s.resize( ToMesh->m_UV0s.size( ) + Mesh->mNumVertices, { 0.0f, 0.0f } );
                     FeatureUV0_Found: { };
                     }

                     /*
                      *
                      * Indices buffer
                      *
                      * */
                     // Base on the assumption that mesh only contains triangles
                     size_t     NumberOfIndices = Mesh->mNumFaces * 3;
                     const auto OldIndicesSize  = ToMesh->m_Indices.size( );
                     {
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

    {   // m_SubMeshes filling
        auto* BeginVer = ToMesh->m_Vertices_ColorPack.data( );

        for ( auto& SubMesh : ToMesh->m_SubMeshes )
        {
            SubMesh.VertexRange = { BeginVer, SubMesh.VertexRange.size( ) };
            BeginVer += SubMesh.VertexRange.size( );
        }

        REQUIRED( BeginVer == ToMesh->m_Vertices_ColorPack.data( ) + ToMesh->m_Vertices_ColorPack.size( ),
                  throw std::runtime_error( "SubMeshesRecord is not valid" ) )
    }

    /*
     *
     * Send buffer to GPU
     *
     * */
    std::vector<GLBufferRange> BufferSpans;
    BufferSpans.emplace_back( ToMesh->m_Vertices_ColorPack );
    if ( ToMesh->m_VertexFeatures & eFeatureNormal ) BufferSpans.emplace_back( ToMesh->m_Normals );
    if ( ToMesh->m_VertexFeatures & eFeatureUV0 ) BufferSpans.emplace_back( ToMesh->m_UV0s );

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    auto& GLBuffer = ToMesh->GetGLBufferHandle( );
    GL_CHECK( gl->GenVertexArrays( 1, &GLBuffer.VAO ) )
    GL_CHECK( gl->BindVertexArray( GLBuffer.VAO ) )
    GL_CHECK( gl->GenBuffers( 1, &GLBuffer.VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, GLBuffer.VBO ) )

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
