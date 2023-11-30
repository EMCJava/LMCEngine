//
// Created by samsa on 11/21/2023.
//

#pragma once

#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>

#include <filesystem>
#include <concepts>
#include <utility>
#include <vector>
#include <string>
#include <memory>

namespace physx
{
class PxDefaultMemoryOutputStream;
class PxGeometryHolder;
class PxRigidDynamic;
class PxRigidStatic;
class PxMaterial;
class PxBase;
}   // namespace physx

class PhysicsEngine;
template <typename Span>
concept SpanLike = requires( Span span ) {
    {
        span.size( )
    } -> std::convertible_to<std::size_t>;
    {
        span.data( )
    };
};
class ColliderSerializerGroupMesh
{
public:
    enum class ColliderType : uint8_t {
        eNone     = 0,
        eConvex   = 1,
        eTriangle = 2
    };

private:
    struct PhysicsMeshInstance {
        std::pair<std::ptrdiff_t, size_t> BufferSpan { };
        ColliderType                      Type   = ColliderType::eNone;
        physx::PxBase*                    PxMesh = nullptr;

        PhysicsMeshInstance( ) = default;
        PhysicsMeshInstance( std::pair<std::ptrdiff_t, size_t> BufferSpan, ColliderType Type, physx::PxBase* PxMesh = nullptr );
        ~PhysicsMeshInstance( );

        PhysicsMeshInstance( PhysicsMeshInstance&& Other ) noexcept { *this = std::move( Other ); }
        PhysicsMeshInstance& operator=( PhysicsMeshInstance&& Other ) noexcept;
        PhysicsMeshInstance( const PhysicsMeshInstance& )            = delete;   // non construction-copyable
        PhysicsMeshInstance& operator=( const PhysicsMeshInstance& ) = delete;   // non copyable

        [[nodiscard]] physx::PxGeometryHolder
        GenerateGeometry( ) const;

        void
        GenerateMeshIfNull( PhysicsEngine* PhyEngine, uint8_t* Data );

        /*
         *
         * Generate and append render ready buffer
         *
         * */
        void
        AppendRenderBuffer( std::vector<float>& Vertices, std::vector<uint32_t>& Indices ) const;
    };

    void
    PushCookedBuffer( physx::PxDefaultMemoryOutputStream& CookingBuffer, ColliderType CType );

public:
    /*
     *
     * This will not load or generate any data
     *
     * */
    ColliderSerializerGroupMesh( std::string Name );

    /*
     *
     * This will load the cached data from file or, generate it and write to file
     * Mesh: any pointer like type to ConceptMesh
     * */
    template <typename Mapping = void*>
    ColliderSerializerGroupMesh( const auto& Mesh, Mapping&& ColliderMapping = nullptr )
        : ColliderSerializerGroupMesh( GetHashFilePath( Mesh->GetFilePath( ) ) )
    {
        auto& SubMeshes  = Mesh->GetSubMeshes( );
        bool  ShouldLoad = !TryLoad( ) || !IsColliderTypeCorrect( SubMeshes, ColliderMapping );
        if ( ShouldLoad )
        {
            spdlog::info( "Number of ModelSubMeshSpan: {}", SubMeshes.size( ) );
            GenerateFrom( SubMeshes, ColliderMapping );
            WriteCacheToFile( );
        }
    }

    static std::string
    GetHashFilePath( std::string_view FilePath );

    std::filesystem::path
    GetFilePath( );

    /*
     *
     * Try loading from offline saved data
     *
     * */
    bool TryLoad( );

    bool IsColliderTypeCorrect( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping );

    void
    GenerateFrom( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping );

    struct CountStrideData {
        size_t      Count  = 0;
        size_t      Stride = 0;
        const void* Data   = nullptr;
    };
    void AppendConvexCache( CountStrideData VertexBuffer, uint16_t ConvexVertexLimit = 100 );
    void AppendTriangleCache( CountStrideData VertexBuffer, CountStrideData IndexBuffer );

    template <SpanLike Span>
    void
    AppendConvexCache( const Span& VertexBuffer, uint16_t ConvexVertexLimit = 100 )
    {
        AppendConvexCache( { VertexBuffer.size( ), sizeof( VertexBuffer[ 0 ] ), (void*) VertexBuffer.data( ) }, ConvexVertexLimit );
    }

    template <SpanLike VertexSpan, SpanLike IndexSpan>
    void
    AppendTriangleCache( const VertexSpan& VertexBuffer, const IndexSpan& IndexBuffer )
    {
        AppendTriangleCache( { VertexBuffer.size( ), sizeof( VertexBuffer[ 0 ] ), (void*) VertexBuffer.data( ) },
                             { IndexBuffer.size( ), sizeof( IndexBuffer[ 0 ] ), (void*) IndexBuffer.data( ) } );
    }

    physx::PxRigidDynamic*
    CreateDynamicRigidBodyFromCacheGroup( const physx::PxMaterial& material, std::shared_ptr<class RenderableMeshHitBox>* HitBoxFrame = nullptr );

    physx::PxRigidStatic*
    CreateStaticRigidBodyFromCacheGroup( const physx::PxMaterial& material, std::shared_ptr<class RenderableMeshHitBox>* HitBoxFrame = nullptr );

    void WriteCacheToFile( );

    void
    ClearBuffer( );

    bool
    HasData( );

    const auto&
    GetCacheBuffer( ) const { return m_CacheBuffer; }

private:
    const std::filesystem::path PATH_PREFIX = "Assets/Physics/Cache/Convexes/";
    std::string                 m_GroupName { };

    // [buffer size(size_t), buffer type(ColliderType), buffer], ...
    std::vector<uint8_t>             m_CacheBuffer;
    std::vector<PhysicsMeshInstance> m_PxMeshInstances;

    static constexpr ColliderType DefaultColliderType = ColliderType::eConvex;

    /*
     *
     * Physx
     *
     * */
    PhysicsEngine* m_PhyEngine = nullptr;
};

bool
ColliderSerializerGroupMesh::IsColliderTypeCorrect( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping )
{
    // Check if mapping is the same
    if ( ModelSubMeshSpan.size( ) != m_PxMeshInstances.size( ) )
        return false;

    for ( int i = 0; i < ModelSubMeshSpan.size( ); ++i )
    {
        if constexpr ( requires { { ColliderMapping( ModelSubMeshSpan[ i ] ) } -> std::convertible_to<ColliderSerializerGroupMesh::ColliderType>; } )
        {
            ColliderSerializerGroupMesh::ColliderType Type = ColliderMapping( ModelSubMeshSpan[ i ] );

            if ( Type != m_PxMeshInstances[ i ].Type )
            {
                return false;
            }
        } else
        {
            if ( DefaultColliderType != m_PxMeshInstances[ i ].Type )
            {
                return false;
            }
        }
    }

    return true;
}

void
ColliderSerializerGroupMesh::GenerateFrom( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping )
{
    ClearBuffer( );
    for ( auto& SubMeshSpan : ModelSubMeshSpan )
    {
        try
        {
            if constexpr ( requires { { ColliderMapping( SubMeshSpan ) } -> std::convertible_to<ColliderSerializerGroupMesh::ColliderType>; } )
            {
                ColliderSerializerGroupMesh::ColliderType Type = ColliderMapping( SubMeshSpan );
                switch ( Type )
                {
                case ColliderType::eNone: break;
                case ColliderType::eConvex:
                    AppendConvexCache( SubMeshSpan.VertexRange );
                    break;
                case ColliderType::eTriangle:
                    AppendTriangleCache( SubMeshSpan.VertexRange, SubMeshSpan.RebasedIndexRange );
                    break;
                }
            } else
            {
                spdlog::warn( "Using convex mesh by default" );
                AppendConvexCache( SubMeshSpan.VertexRange );
            }
        }
        catch ( std::runtime_error& e )
        {
            spdlog::error( "Failed to generate model: {}", e.what( ) );
        }
    }
}