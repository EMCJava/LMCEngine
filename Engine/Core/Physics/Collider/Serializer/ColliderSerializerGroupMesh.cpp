//
// Created by samsa on 11/21/2023.
//

#include "ColliderSerializerGroupMesh.hpp"

#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>

#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <fstream>

using ColliderType = ColliderSerializerGroupMesh::ColliderType;

namespace
{
template <typename Ty>
void
BufferReadValue( auto*& Buffer, Ty& Value )
{
    Value = *reinterpret_cast<Ty*>( Buffer );

    static_assert( sizeof( Buffer[ 0 ] ) == sizeof( char ) );
    Buffer += sizeof( Ty );
}
}   // namespace

void
ColliderSerializerGroupMesh::PushCookedBuffer( physx::PxDefaultMemoryOutputStream& CookingBuffer, ColliderType CType )
{
    const auto OldSize = m_CacheBuffer.size( );
    m_CacheBuffer.resize( OldSize + sizeof( size_t ) + sizeof( ColliderType ) + CookingBuffer.getSize( ) );

    auto* ChunkStart = m_CacheBuffer.data( ) + OldSize;

    // Buffer size
    size_t BufferSize = CookingBuffer.getSize( );
    memcpy( ChunkStart, &BufferSize, sizeof( size_t ) );
    // Buffer type
    *reinterpret_cast<ColliderType*>( ChunkStart + sizeof( size_t ) ) = CType;
    // Buffer data
    memcpy( ChunkStart + sizeof( size_t ) + sizeof( ColliderType ), CookingBuffer.getData( ), CookingBuffer.getSize( ) );

    // Skip list save
    m_PxMeshInstances.emplace_back( std::pair { OldSize + sizeof( size_t ) + sizeof( ColliderType ), CookingBuffer.getSize( ) }, CType );

    spdlog::info( "Cooked buffer size: {}[{}]", CookingBuffer.getSize( ), (uint8_t) CType );
}

std::string
ColliderSerializerGroupMesh::GetHashFilePath( std::string_view FilePath )
{
    return std::to_string( hash_value( std::filesystem::path( FilePath ) ) );
}

std::filesystem::path
ColliderSerializerGroupMesh::GetFilePath( )
{
    return PATH_PREFIX / ( m_GroupName + "_group_mesh_cache.bin" );
}

bool
ColliderSerializerGroupMesh::TryLoad( )
{
    const auto CachePath = GetFilePath( );

    std::error_code ErrorCode;
    // Cache already exists
    if ( std::filesystem::exists( CachePath.parent_path( ), ErrorCode ) && std::filesystem::exists( CachePath ) )
    {
        std::ifstream InputStream;
        InputStream.open( CachePath, std::ios::in | std::ios::binary | std::ios::ate );

        const auto BufferSize = InputStream.tellg( );
        InputStream.seekg( 0, std::ios::beg );

        m_PxMeshInstances.clear( );
        m_CacheBuffer.resize( BufferSize );
        InputStream.read( (std::ifstream::char_type*) m_CacheBuffer.data( ), BufferSize );
        if ( BufferSize != InputStream.gcount( ) ) throw std::runtime_error( "Failed to read convex.bin" );

        auto* BufferBegin = m_CacheBuffer.data( );
        auto* BufferEnd   = BufferBegin + BufferSize;

        while ( BufferBegin != BufferEnd )
        {
            size_t       SpanSize;
            ColliderType CType;

            BufferReadValue( BufferBegin, SpanSize );
            BufferReadValue( BufferBegin, CType );

            m_PxMeshInstances.emplace_back( std::pair { std::distance( m_CacheBuffer.data( ), BufferBegin ), SpanSize }, CType );
            BufferBegin += SpanSize;
        }


        return true;
    }

    return false;
}

void
ColliderSerializerGroupMesh::AppendConvexCache( ColliderSerializerGroupMesh::CountStrideData VertexBuffer, uint16_t ConvexVertexLimit )
{
    physx::PxConvexMeshDesc convexDesc;
    convexDesc.points.count  = VertexBuffer.Count;
    convexDesc.points.stride = VertexBuffer.Stride;
    convexDesc.points.data   = VertexBuffer.Data;
    convexDesc.flags         = physx::PxConvexFlag::eCOMPUTE_CONVEX
        | physx::PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES
        | physx::PxConvexFlag::eSHIFT_VERTICES;   // Need to verify
    convexDesc.vertexLimit = ConvexVertexLimit;

    physx::PxDefaultMemoryOutputStream     CookingBuffer;
    physx::PxConvexMeshCookingResult::Enum result;

    auto* PhysxCooking = m_PhyEngine->GetCooking( );
    auto  TriParams = PhysxCooking->getParams( ), ParamsCopy = TriParams;

    TriParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
    TriParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
    m_PhyEngine->GetCooking( )->setParams( TriParams );

    if ( !m_PhyEngine->GetCooking( )->cookConvexMesh( convexDesc, CookingBuffer, &result ) )
    {
        m_PhyEngine->GetCooking( )->setParams( ParamsCopy );
        throw std::runtime_error( "Failed to cook convex mesh" );
    }

    m_PhyEngine->GetCooking( )->setParams( ParamsCopy );
    PushCookedBuffer( CookingBuffer, ColliderType::eConvex );
}

void
ColliderSerializerGroupMesh::AppendTriangleCache( ColliderSerializerGroupMesh::CountStrideData VertexBuffer, ColliderSerializerGroupMesh::CountStrideData IndexBuffer )
{
    REQUIRED( IndexBuffer.Count % 3 == 0, return; )

    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count  = VertexBuffer.Count;
    meshDesc.points.stride = VertexBuffer.Stride;
    meshDesc.points.data   = VertexBuffer.Data;

    meshDesc.triangles.count  = IndexBuffer.Count / 3;
    meshDesc.triangles.stride = 3 * IndexBuffer.Stride;
    meshDesc.triangles.data   = IndexBuffer.Data;

    physx::PxDefaultMemoryOutputStream       CookingBuffer;
    physx::PxTriangleMeshCookingResult::Enum result;
    if ( !m_PhyEngine->GetCooking( )->cookTriangleMesh( meshDesc, CookingBuffer, &result ) )
        throw std::runtime_error( "Failed to cook triangle mesh" );

    PushCookedBuffer( CookingBuffer, ColliderType::eTriangle );
}

physx::PxRigidDynamic*
ColliderSerializerGroupMesh::CreateDynamicRigidBodyFromCacheGroup( const physx::PxMaterial& material )
{
    auto* hfActor = ( *m_PhyEngine )->createRigidDynamic( physx::PxTransform( physx::PxVec3( 0 ) ) );
    hfActor->setName( m_GroupName.c_str( ) );

    for ( auto& PxMeshInstance : m_PxMeshInstances )
    {
        PxMeshInstance.GenerateMeshIfNull( m_PhyEngine, m_CacheBuffer.data( ) );
        if ( PxMeshInstance.Type != ColliderType::eConvex )
        {
            spdlog::warn( "Skipping non-convex mesh for dynamic rigid body(not supported by Physx)." );
            continue;
        }

        physx::PxShape* aShape = physx::PxRigidActorExt::createExclusiveShape( *hfActor, PxMeshInstance.GenerateGeometry( ).any( ), material );
        REQUIRED( aShape != nullptr )
    }

    return hfActor;
}

physx::PxRigidStatic*
ColliderSerializerGroupMesh::CreateStaticRigidBodyFromCacheGroup( const physx::PxMaterial& material )
{
    auto* hfActor = ( *m_PhyEngine )->createRigidStatic( physx::PxTransform( physx::PxVec3( 0 ) ) );
    hfActor->setName( m_GroupName.c_str( ) );

    for ( auto& PxMeshInstance : m_PxMeshInstances )
    {
        PxMeshInstance.GenerateMeshIfNull( m_PhyEngine, m_CacheBuffer.data( ) );
        if ( PxMeshInstance.Type == ColliderType::eNone )
        {
            spdlog::warn( "Skipping non-defined mesh type for rigid body." );
            continue;
        }

        physx::PxShape* aShape = physx::PxRigidActorExt::createExclusiveShape( *hfActor, PxMeshInstance.GenerateGeometry( ).any( ), material );
        REQUIRED( aShape != nullptr )
    }

    return hfActor;
}

void
ColliderSerializerGroupMesh::WriteCacheToFile( )
{
    const auto CachePath = GetFilePath( );

    std::error_code ErrorCode;
    !std::filesystem::exists( CachePath.parent_path( ), ErrorCode ) && std::filesystem::create_directories( CachePath.parent_path( ), ErrorCode );

    // Cache already exists
    if ( std::filesystem::exists( CachePath ) )
    {
        spdlog::warn( "Cache file already exists, overwriting" );
    }

    std::ofstream OutputStream;
    OutputStream.open( CachePath, std::ios::out | std::ios::binary );
    if ( !OutputStream.write( (std::ofstream::char_type*) m_CacheBuffer.data( ), m_CacheBuffer.size( ) ) )
    {
        throw std::runtime_error( "Failed to write convex cache" );
    }
}

void
ColliderSerializerGroupMesh::ClearBuffer( )
{
    m_PxMeshInstances.clear( );
    m_CacheBuffer.clear( );
}

bool
ColliderSerializerGroupMesh::HasData( )
{
    REQUIRED( m_CacheBuffer.empty( ) == m_PxMeshInstances.empty( ) )
    return !m_CacheBuffer.empty( );
}

ColliderSerializerGroupMesh::ColliderSerializerGroupMesh( std::string Name )
    : m_GroupName( std::move( Name ) )
{
    m_PhyEngine = Engine::GetEngine( )->GetPhysicsEngine( );
}

ColliderSerializerGroupMesh::PhysicsMeshInstance::PhysicsMeshInstance( std::pair<std::ptrdiff_t, size_t> BufferSpan, ColliderType Type, physx::PxBase* PxMesh )
    : BufferSpan( BufferSpan )
    , Type( Type )
    , PxMesh( PxMesh )
{ }

ColliderSerializerGroupMesh::PhysicsMeshInstance::~PhysicsMeshInstance( )
{
    if ( PxMesh != nullptr )
    {
        PxMesh->release( );
        PxMesh = nullptr;
    }
}

ColliderSerializerGroupMesh::PhysicsMeshInstance&
ColliderSerializerGroupMesh::PhysicsMeshInstance::operator=( ColliderSerializerGroupMesh::PhysicsMeshInstance&& Other ) noexcept
{
    BufferSpan = std::move( Other.BufferSpan );
    if ( PxMesh != nullptr )
    {
        PxMesh->release( );
        PxMesh = nullptr;
    }

    Type = Other.Type;
    std::swap( PxMesh, Other.PxMesh );

    return *this;
}

physx::PxGeometryHolder
ColliderSerializerGroupMesh::PhysicsMeshInstance::GenerateGeometry( ) const
{
    REQUIRED( PxMesh != nullptr, throw std::runtime_error( "PxMesh is nullptr" ) )
    switch ( Type )
    {
    case ColliderType::eNone:
        REQUIRED( false, throw std::runtime_error( "Geometry from type none" ) )
    case ColliderType::eConvex:
        return physx::PxConvexMeshGeometry( (physx::PxConvexMesh*) PxMesh );
    case ColliderType::eTriangle:
        return physx::PxTriangleMeshGeometry( (physx::PxTriangleMesh*) PxMesh );
    }
}

void
ColliderSerializerGroupMesh::PhysicsMeshInstance::GenerateMeshIfNull( PhysicsEngine* PhyEngine, uint8_t* Data )
{
    if ( PxMesh != nullptr ) return;

    physx::PxDefaultMemoryInputData PxMemoryBuffer( Data + BufferSpan.first, BufferSpan.second );
    switch ( Type )
    {
    case ColliderType::eNone: break;
    case ColliderType::eConvex: {
        PxMesh = ( *PhyEngine )->createConvexMesh( PxMemoryBuffer );
        break;
    }
    case ColliderType::eTriangle:
        PxMesh = ( *PhyEngine )->createTriangleMesh( PxMemoryBuffer );
        break;
    }
}
