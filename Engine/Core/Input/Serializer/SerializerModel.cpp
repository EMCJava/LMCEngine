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

    ToMesh->m_FilePath = m_FilePath;
    ToMesh->m_Vertices_ColorPack.clear( );
    ToMesh->m_Normals.clear( );
    ToMesh->m_Indices.clear( );
    ToMesh->m_SubMeshes.clear( );

    std::vector<std::shared_ptr<Material>> ModelMaterials { m_ModelScene->mNumMaterials };
    for ( auto& Mat : ModelMaterials )
        Mat = std::make_shared<Material>( );

    for ( int i = 0; i < m_ModelScene->mNumMaterials; ++i )
    {
        auto* Material = m_ModelScene->mMaterials[ i ];

        /*        {
                    std::string Keys = "\n";
                    for ( int np = 0; np < Material->mNumProperties; ++np )
                    {
                        Keys += Material->mProperties[ np ]->mKey.C_Str( );
                        Keys += ": ";
                        switch ( Material->mProperties[ np ]->mType )
                        {
                        case aiPTI_Float:
                            Keys += std::to_string( *(float*) Material->mProperties[ np ]->mData );
                            break;
                        case aiPTI_Double:
                            Keys += std::to_string( *(double*) Material->mProperties[ np ]->mData );
                            break;
                        case aiPTI_String:
                            Keys += ( (aiString*) Material->mProperties[ np ]->mData )->C_Str( );
                            break;
                        case aiPTI_Integer:
                            Keys += std::to_string( *(int*) Material->mProperties[ np ]->mData );
                            break;
                        case aiPTI_Buffer:
                            Keys += "BUFFER";
                            break;
                        case _aiPTI_Force32Bit:
                            Keys += std::to_string( *(int32_t*) Material->mProperties[ np ]->mData );
                            break;
                        }
                        Keys += "\n";
                    }
                    spdlog::info( "Material${} Properties: {}", i, Keys );
                }*/

        aiString DiffuseTexturePath;

        if ( Material->Get( AI_MATKEY_TEXTURE_DIFFUSE( 0 ), DiffuseTexturePath ) == AI_SUCCESS )
        {
            using namespace std::filesystem;
            path TextureCompletePath = ToMesh->GetFilePath( );
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
    }

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
                     spdlog::warn( "> Ignoring {} texture(s) on Material${}", Material->GetTextureCount( aiTextureType_DIFFUSE ), Mesh->mMaterialIndex );

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

    spdlog::info( "Finished loading model ===================={}====================", m_FilePath );

    return true;
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
    }

    ::FetchMesh( m_ModelScene,
                 m_ModelScene->mRootNode,
                 m_ModelScene->mRootNode->mTransformation,
                 [ this, &RenderableMeshes, &ModelMaterials, VertexFeature ]( aiScene const* ModelScene, aiMesh* AIMesh, const aiMatrix4x4& Transform ) {
                     static_assert( sizeof( glm::vec3 ) == sizeof( decltype( *AIMesh->mVertices ) ) );

                     auto NewMesh           = ConceptMesh::CreateConcept( );
                     auto NewRenderableMesh = RenderableMesh::CreateConcept( NewMesh );
                     RenderableMeshes.push_back( NewRenderableMesh );

                     NewMesh->m_FilePath = m_FilePath;
                     NewMesh->m_Vertices_ColorPack.clear( );
                     NewMesh->m_Normals.clear( );
                     NewMesh->m_Indices.clear( );
                     NewMesh->m_SubMeshes.clear( );

                     NewMesh->m_VertexFeatures = (MeshVertexFeature) VertexFeature;

                     /*
                      *
                      * Material setup
                      *
                      * */
                     auto*     Material         = ModelScene->mMaterials[ AIMesh->mMaterialIndex ];
                     aiColor3D DiffuseColor     = { 1, 0, 0.86 };
                     aiReturn  ColorFetchResult = Material->Get( AI_MATKEY_COLOR_DIFFUSE, DiffuseColor );
                     if ( ColorFetchResult != AI_SUCCESS ) spdlog::warn( "No diffuse color defined in material:{}", Material->GetName( ).C_Str( ) );

                     float ColorPack = PackColor( DiffuseColor );

                     /*
                      *
                      * Texture setup
                      *
                      * */
                     NewRenderableMesh->SetMaterial( ModelMaterials[ AIMesh->mMaterialIndex ] );

                     /*
                      *
                      * Vertex
                      *
                      * */
                     REQUIRED( NewMesh->m_VertexFeatures & eFeaturePosition, throw std::runtime_error( "Vertex position is required" ) );
                     {
                         NewMesh->m_Vertices_ColorPack.resize( AIMesh->mNumVertices );

                         for ( int i = 0; i < AIMesh->mNumVertices; ++i )
                         {
                             // Little hack
                             *(aiVector3D*) ( &NewMesh->m_Vertices_ColorPack[ i ] ) = Transform * AIMesh->mVertices[ i ];
                             NewMesh->m_Vertices_ColorPack[ i ].w                   = ColorPack;
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
                     NewMesh->m_SubMeshes.emplace_back( AIMesh->mName.C_Str( ), NewMesh->m_Vertices_ColorPack, NewMesh->m_Indices );

                     /*
                      *
                      * Send buffer to GPU
                      *
                      * */
                     std::vector<GLBufferRange> BufferSpans;
                     BufferSpans.emplace_back( NewMesh->m_Vertices_ColorPack );
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
SerializerModel::ToMesh( const std::string& FilePath, ConceptMesh* ToMesh )
{
    SerializerModel Tmp;
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
SerializerModel::ToMesh( const std::string& FilePath )
{
    auto CM = PureConcept::CreateConcept<ConceptMesh>( );
    if ( SerializerModel::ToMesh( FilePath, CM.get( ) ) )
    {
        return CM;
    }

    return nullptr;
}
