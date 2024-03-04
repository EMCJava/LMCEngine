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
#include <Engine/Core/Graphic/Material/Material.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMeshCluster.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>

#include <assimp/Importer.hpp>    // C++ importer interface
#include <assimp/scene.h>         // Output data structure
#include <assimp/postprocess.h>   // Post processing flags

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <string>
#include <numeric>
#include <filesystem>

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

namespace
{
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
}   // namespace

bool
SerializerModel::ToMesh( ConceptMesh* ToMesh )
{
    if ( !m_ModelScene && !LoadModel( ) )
    {
        spdlog::error( "Failed loading model ===================={}====================", m_FilePath );
        return false;
    }

    spdlog::info( "Loading model ===================={}====================", m_FilePath );

    /*
     *
     * First read as Cluster then combine
     *
     * */

    auto MeshCluster = PureConcept::CreateConcept<RenderableMeshCluster>( );
    if ( !ToMeshCluster( MeshCluster.get( ), ToMesh->m_VertexFeatures ) )
    {
        return false;
    }

    bool result = SerializerModel::ToMesh( MeshCluster, ToMesh );
    spdlog::info( "Finished loading model ===================={}====================", m_FilePath );

    return result;
}

bool
SerializerModel::ToMeshCluster( RenderableMeshCluster* ToMeshCluster, uint32_t VertexFeature )
{
    if ( !m_ModelScene && !LoadModel( ) )
    {
        spdlog::error( "Failed loading model ===================={}====================", m_FilePath );
        return false;
    }

    spdlog::info( "Loading model cluster ===================={}====================", m_FilePath );

    std::vector<std::shared_ptr<RenderableMesh>> RenderableMeshes;

    std::vector<std::shared_ptr<Material>> ModelMaterials { m_ModelScene->mNumMaterials };
    for ( auto& Mat : ModelMaterials )
        Mat = std::make_shared<Material>( );

    for ( int i = 0; i < m_ModelScene->mNumMaterials; ++i )
    {
        auto* Material = m_ModelScene->mMaterials[ i ];

        /*
         *
         * Diffuse texture
         *
         * */
        aiString DiffuseTexturePath;
        if ( Material->Get( AI_MATKEY_TEXTURE_DIFFUSE( 0 ), DiffuseTexturePath ) == AI_SUCCESS )
        {

            const auto TextureCount = Material->GetTextureCount( aiTextureType_DIFFUSE );
            if ( TextureCount > 1 ) [[unlikely]]
                spdlog::warn( "> Ignoring {}-1 texture(s) on Material${}", TextureCount, i );

            using namespace std::filesystem;
            path TextureCompletePath = m_FilePath;
            if ( TextureCompletePath.is_relative( ) )
            {
                const auto ModelParentPath = absolute( TextureCompletePath.parent_path( ) );
                if ( exists( ModelParentPath ) )
                {
                    TextureCompletePath = ModelParentPath / DiffuseTexturePath.C_Str( );
                    if ( !exists( TextureCompletePath ) )
                    {
                        TextureCompletePath.clear( );
                    }
                } else
                {
                    TextureCompletePath.clear( );
                }

            } else if ( !exists( TextureCompletePath ) )
            {
                TextureCompletePath.clear( );
            }

            if ( !TextureCompletePath.empty( ) )
            {
                ModelMaterials[ i ]->ColorTexture.LoadTexture( TextureCompletePath.string( ) );
            }
        }

        /*
         *
         * Diffuse color
         *
         * */
        aiReturn ColorFetchResult = Material->Get( AI_MATKEY_COLOR_DIFFUSE, *(aiColor3D*) &ModelMaterials[ i ]->DiffuseColor );
        if ( ColorFetchResult != AI_SUCCESS ) spdlog::warn( "> No diffuse color defined in on Material${}", i );
    }

    ::FetchMesh( m_ModelScene,
                 m_ModelScene->mRootNode,
                 *(aiMatrix4x4*) &m_GlobalTransform * m_ModelScene->mRootNode->mTransformation,
                 [ this, &RenderableMeshes, &ModelMaterials, VertexFeature ]( aiScene const* ModelScene, aiMesh* AIMesh, const aiMatrix4x4& Transform ) {
                     static_assert( sizeof( glm::vec3 ) == sizeof( decltype( *AIMesh->mVertices ) ) );

                     auto NewMesh           = ConceptMesh::CreateConcept( );
                     auto NewRenderableMesh = RenderableMesh::CreateConcept( NewMesh );
                     RenderableMeshes.push_back( NewRenderableMesh );

                     NewMesh->m_FilePath = m_FilePath;
                     NewMesh->m_Vertices.clear( );
                     NewMesh->m_Normals.clear( );
                     NewMesh->m_Indices.clear( );
                     NewMesh->m_SubMeshes.clear( );

                     NewMesh->m_VertexFeatures = (MeshVertexFeature) VertexFeature;

                     /*
                      *
                      * Material setup
                      *
                      * */
                     NewRenderableMesh->SetMaterial( ModelMaterials[ AIMesh->mMaterialIndex ] );

                     /*
                      *
                      * Vertex
                      *
                      * */
                     REQUIRED( VertexFeature & eFeaturePosition, throw std::runtime_error( "Vertex position is required" ) );
                     REQUIRED( NewMesh->m_VertexFeatures & eFeaturePosition, throw std::runtime_error( "Vertex position is required" ) );
                     {
                         NewMesh->m_Vertices.resize( AIMesh->mNumVertices );
                         for ( int i = 0; i < AIMesh->mNumVertices; ++i )
                         {
                             // Little hack
                             *(aiVector3D*) ( &NewMesh->m_Vertices[ i ] ) = Transform * AIMesh->mVertices[ i ];
                         }
                     }

                     /*
                      *
                      *  Normal buffer
                      *
                      * */
                     if ( NewMesh->m_VertexFeatures & eFeatureNormal )
                     {
                         auto* NormalBegin = (glm::vec3*) AIMesh->mNormals;
                         auto* NormalEnd   = (glm::vec3*) AIMesh->mNormals + AIMesh->mNumVertices;
                         NewMesh->m_Normals.assign( NormalBegin, NormalEnd );
                     }

                     /*
                      *
                      *  UV buffer
                      *
                      * */
                     if ( NewMesh->m_VertexFeatures & eFeatureUV0 )
                     {
                         const auto TextureIndex = 0;
                         if ( AIMesh->mTextureCoords[ TextureIndex ] != nullptr )
                         {
                             REQUIRED_IF( AIMesh->mNumUVComponents[ TextureIndex ] == 2 )   // glm::vec2
                             {
                                 NewMesh->m_UV0s.resize( AIMesh->mNumVertices );
                                 // Can't just assign as the library use 3d coordinates by default
                                 for ( int i = 0; i < AIMesh->mNumVertices; ++i )
                                     NewMesh->m_UV0s[ i ] = *(glm::vec2*) &AIMesh->mTextureCoords[ TextureIndex ][ i ];

                                 goto FeatureUV0_Found;
                             }
                         }

                         // Default fill if no UV available
                         NewMesh->m_UV0s.resize( AIMesh->mNumVertices, { 0.0f, 0.0f } );
                     FeatureUV0_Found: { };
                     }

                     /*
                      *
                      * Indices buffer
                      *
                      * */

                     {
                         NewMesh->m_Indices.resize( AIMesh->mNumFaces * 3 /* mesh only contains triangles */ );
                         uint32_t* IndicesData = NewMesh->m_Indices.data( );

                         for ( int j = 0; j < AIMesh->mNumFaces; ++j )
                         {
                             for ( int k = 0; k < 3 /* For optimize */; ++k )
                             {
                                 IndicesData[ k ] = AIMesh->mFaces[ j ].mIndices[ k ];
                             }

                             IndicesData += 3;
                         }
                     }

                     // Keep submesh record for entire span
                     NewMesh->m_SubMeshes.emplace_back( AIMesh->mName.C_Str( ), NewMesh->m_Vertices, NewMesh->m_Indices );

                     /*
                      *
                      * Send buffer to GPU
                      *
                      * */
                     std::vector<GLBufferRange> BufferSpans;
                     BufferSpans.emplace_back( NewMesh->m_Vertices );
                     if ( NewMesh->m_VertexFeatures & eFeatureNormal ) BufferSpans.emplace_back( NewMesh->m_Normals );
                     if ( NewMesh->m_VertexFeatures & eFeatureUV0 ) BufferSpans.emplace_back( NewMesh->m_UV0s );

                     const auto* gl = Engine::GetEngine( )->GetGLContext( );
                     GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

                     auto& GLBuffer = NewMesh->GetGLBufferHandle( );
                     GL_CHECK( gl->GenVertexArrays( 1, &GLBuffer.VAO ) )
                     GL_CHECK( gl->BindVertexArray( GLBuffer.VAO ) )
                     GL_CHECK( gl->GenBuffers( 1, &GLBuffer.VBO ) )
                     GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, GLBuffer.VBO ) )

                     /*
                      *
                      * Combine all buffers into one buffer for GPU
                      *
                      * */
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
                     GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint32_t ) * NewMesh->m_Indices.size( ), NewMesh->m_Indices.data( ), GL_STATIC_DRAW ) )
                 } );

    for ( auto& RM : RenderableMeshes )
    {
        ToMeshCluster->GetOwnership( RM );
    }

    spdlog::info( "Finished loading model cluster ===================={}====================", m_FilePath );

    return true;
}

bool
SerializerModel::ToMesh( const std::string& FilePath, ConceptMesh* ToMesh, const glm::mat4& Transform )
{
    SerializerModel Tmp;
    Tmp.SetGlobalTransform( Transform );
    Tmp.SetFilePath( FilePath );
    return Tmp.ToMesh( ToMesh );
}

bool
SerializerModel::ToMeshCluster( const std::string& FilePath, RenderableMeshCluster* ToMesh, uint32_t VertexFeature )
{
    SerializerModel Tmp;
    Tmp.SetFilePath( FilePath );
    return Tmp.ToMeshCluster( ToMesh, VertexFeature );
}

std::shared_ptr<RenderableMeshCluster>
SerializerModel::ToMeshCluster( const std::string& FilePath, uint32_t VertexFeature )
{
    auto RMCluster = PureConcept::CreateConcept<RenderableMeshCluster>( );
    if ( SerializerModel::ToMeshCluster( FilePath, RMCluster.get( ), VertexFeature ) )
    {
        return RMCluster;
    }

    return nullptr;
}

std::shared_ptr<ConceptMesh>
SerializerModel::ToMesh( const std::string& FilePath, const glm::mat4& Transform )
{
    auto CM = PureConcept::CreateConcept<ConceptMesh>( );
    if ( SerializerModel::ToMesh( FilePath, CM.get( ), Transform ) )
    {
        return CM;
    }

    return nullptr;
}

std::shared_ptr<class ConceptMesh>
SerializerModel::ToMesh( const std::shared_ptr<RenderableMeshCluster>& MeshCluster )
{
    auto Result = PureConcept::CreateConcept<ConceptMesh>( );

    if ( ToMesh( MeshCluster, Result.get( ) ) )
        return Result;

    return nullptr;
}

bool
SerializerModel::ToMesh( const std::shared_ptr<RenderableMeshCluster>& MeshCluster, ConceptMesh* ToMesh )
{
    std::vector<std::shared_ptr<RenderableMesh>> RenderableMeshes;
    MeshCluster->GetConcepts( RenderableMeshes );

    ToMesh->m_Vertices.clear( );
    ToMesh->m_Normals.clear( );
    ToMesh->m_Indices.clear( );
    ToMesh->m_SubMeshes.clear( );

    if ( RenderableMeshes.empty( ) ) throw std::runtime_error( "MeshCluster has no meshes" );
    ToMesh->m_FilePath = RenderableMeshes[ 0 ]->GetStaticMesh( )->m_FilePath;

    struct DataCapacity {
        size_t Vertices = 0;
        size_t Normals  = 0;
        size_t Indices  = 0;
        size_t UV0      = 0;
    } ClusterDataCapacity;

    for ( const auto& RM : RenderableMeshes )
    {
        ClusterDataCapacity.Vertices += RM->GetStaticMesh( )->m_Vertices.size( );
        ClusterDataCapacity.Normals += RM->GetStaticMesh( )->m_Normals.size( );
        ClusterDataCapacity.Indices += RM->GetStaticMesh( )->m_Indices.size( );
        ClusterDataCapacity.UV0 += RM->GetStaticMesh( )->m_UV0s.size( );
    }
    ToMesh->m_Vertices.reserve( ClusterDataCapacity.Vertices );
    ToMesh->m_Normals.reserve( ClusterDataCapacity.Normals );
    ToMesh->m_UV0s.reserve( ClusterDataCapacity.UV0 );

    size_t ValidIndexCount = 0;
    ToMesh->m_Indices.resize( ClusterDataCapacity.Indices );

    for ( const auto& RM : RenderableMeshes )
    {
        const auto& RMMesh = RM->GetStaticMesh( );

        // Indices buffer
        {
            uint32_t*  IndicesData = ToMesh->m_Indices.data( ) + ValidIndexCount;
            const auto IndexCount  = RMMesh->m_Indices.size( );
            ValidIndexCount += IndexCount;

            const auto VerticesCount = ToMesh->m_Vertices.size( );
            for ( int i = 0; i < IndexCount; ++i )
                IndicesData[ i ] = RMMesh->m_Indices[ i ] + VerticesCount;
        }

        // Vertices
        ToMesh->m_Vertices.insert( ToMesh->m_Vertices.end( ),
                                   RMMesh->m_Vertices.begin( ),
                                   RMMesh->m_Vertices.end( ) );

        // Normals
        ToMesh->m_Normals.insert( ToMesh->m_Normals.end( ),
                                  RMMesh->m_Normals.begin( ),
                                  RMMesh->m_Normals.end( ) );

        // UV buffer
        ToMesh->m_UV0s.insert( ToMesh->m_UV0s.end( ),
                               RMMesh->m_UV0s.begin( ),
                               RMMesh->m_UV0s.end( ) );

        // Keep submesh record
        ToMesh->m_SubMeshes.emplace_back( RMMesh->m_SubMeshes.front( ).SubMeshName,
                                          std::span<glm::vec3> { &ToMesh->m_Vertices.back( ) - RMMesh->m_Vertices.size( ) + 1, RMMesh->m_Vertices.size( ) },
                                          RMMesh->m_SubMeshes.front( ).RebasedIndexRange );
    }

    // Make sure submesh record is valid
    REQUIRED( ToMesh->m_Vertices.capacity( ) == ClusterDataCapacity.Vertices );
    REQUIRED( ToMesh->m_Normals.capacity( ) == ClusterDataCapacity.Normals );
    REQUIRED( ToMesh->m_Indices.capacity( ) == ClusterDataCapacity.Indices );
    REQUIRED( ToMesh->m_UV0s.capacity( ) == ClusterDataCapacity.UV0 );

    /*
     *
     * Send buffer to GPU
     *
     * */
    std::vector<GLBufferRange> BufferSpans;
    BufferSpans.emplace_back( ToMesh->m_Vertices );
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

    spdlog::info( "Finished converting model ========================================" );

    return true;
}
