//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"

#include "WandEditorScene.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/Serializer/SerializerModel.hpp>
#include <Engine/Core/Graphic/Mesh/ConceptMesh.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include <PxPhysicsAPI.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <ranges>
#include <utility>

DEFINE_CONCEPT_MA_SE( GameManager )
GameManager::~GameManager( )
{
    // FIXME: Tmp fix to avoid PhyEng destructing before all concepts
    RemoveConcepts<PureConcept>( );
}
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, ConceptApplicable, TestInvokable )

class LightRotate : public ConceptApplicable
{
    DECLARE_CONCEPT( LightRotate, ConceptApplicable )
public:
    LightRotate( std::shared_ptr<RenderableMesh> Mesh, std::shared_ptr<RenderableMesh> LightMesh )
        : m_Mesh( std::move( Mesh ) )
        , m_LightMesh( std::move( LightMesh ) )
    {
        m_LightOrientation.SetScale( 0.08, 0.08, 0.08 );
    }

    void
    Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_AccumulatedTime += DeltaTime;

        const auto Location = glm::vec3( cos( m_AccumulatedTime * 2 ) * 5.0f, 2, sin( m_AccumulatedTime * 2 ) * 5.0f );
        m_Mesh->SetShaderUniform( "lightPos", Location );
        m_LightOrientation.SetCoordinate( Location, true );

        m_LightMesh->SetShaderUniform( "modelMatrix", m_LightOrientation.GetModelMatrix( ) );
    }

protected:
    FloatTy                         m_AccumulatedTime { 0.0f };
    std::shared_ptr<RenderableMesh> m_Mesh;
    std::shared_ptr<RenderableMesh> m_LightMesh;

    Orientation m_LightOrientation;
};
DEFINE_CONCEPT_DS( LightRotate )

class CameraController : public ConceptApplicable
{
    DECLARE_CONCEPT( CameraController, ConceptApplicable )

public:
    explicit CameraController( std::shared_ptr<PureConceptPerspectiveCamera> Camera )
        : m_Camera( std::move( Camera ) )
    {
        Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( true );
    }

    void
    Apply( ) override
    {
        const auto    DeltaTime      = Engine::GetEngine( )->GetDeltaSecond( );
        const FloatTy DeltaChange    = DeltaTime * m_ViewControlSensitivity;
        auto          CameraPosition = m_Camera->GetCameraPosition( );

        int FrontMovement = 0;
        int RightMovement = 0;

        auto* UserInputHandle = Engine::GetEngine( )->GetUserInputHandle( );

        const bool Accelerated = UserInputHandle->GetKeyState( GLFW_KEY_LEFT_CONTROL ).isDown;

        if ( UserInputHandle->GetKeyState( GLFW_KEY_W ).isDown ) FrontMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_S ).isDown ) FrontMovement -= 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_D ).isDown ) RightMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_A ).isDown ) RightMovement -= 1;

        if ( Accelerated )
        {
            FrontMovement <<= 4;
            RightMovement <<= 4;
        }

        if ( FrontMovement != 0 ) CameraPosition += m_Camera->GetCameraFacing( ) * ( FrontMovement * DeltaChange );
        if ( RightMovement != 0 ) CameraPosition += m_Camera->GetCameraRightVector( ) * ( RightMovement * DeltaChange );
        if ( FrontMovement || RightMovement ) m_Camera->SetCameraPosition( CameraPosition );

        if ( UserInputHandle->GetKeyState( GLFW_KEY_ESCAPE ).isPressed )
        {
            Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( ( m_MouseLocked = !m_MouseLocked ) );
        }

        if ( m_MouseLocked )
        {
            const auto Delta = UserInputHandle->GetCursorDeltaPosition( );
            // spdlog::info( "Delta movement: {},{}", Delta.first, Delta.second );
            m_Camera->AlterCameraPrincipalAxes( Delta.first * 0.05F, -Delta.second * 0.05F );
        }
    }

protected:
    std::shared_ptr<PureConceptPerspectiveCamera> m_Camera;
    FloatTy                                       m_ViewControlSensitivity { 1.0f };

    bool m_MouseLocked { true };

    ENABLE_IMGUI( CameraController )
};
CameraController::~CameraController( )
{
    Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( false );
}
DEFINE_CONCEPT( CameraController )
DEFINE_SIMPLE_IMGUI_TYPE( CameraController, m_ViewControlSensitivity )


class PhysicsScene
{
    void InitializeScene( );

public:
    PhysicsScene( physx::PxPhysics* Physics )
        : m_Physics( Physics )
    {
        physx::PxSceneDesc sceneDesc( Physics->getTolerancesScale( ) );
        sceneDesc.gravity       = physx::PxVec3( 0.0f, -9.81f, 0.0f );
        m_Dispatcher            = physx::PxDefaultCpuDispatcherCreate( 4 );
        sceneDesc.cpuDispatcher = m_Dispatcher;
        sceneDesc.filterShader  = physx::PxDefaultSimulationFilterShader;
        m_Scene                 = Physics->createScene( sceneDesc );

        // flags
        m_Scene->setFlag( physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true );

        physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient( );
        if ( pvdClient )
        {
            spdlog::info( "Setting PVD flag" );
            pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
            pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
            pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
        }
    }

    operator physx::PxScene&( ) { return *m_Scene; }
    auto* operator->( ) { return m_Scene; }

private:
    physx::PxScene*                m_Scene      = nullptr;
    physx::PxDefaultCpuDispatcher* m_Dispatcher = nullptr;

    physx::PxPhysics* m_Physics = nullptr;
};
class PhysicsEngine
{
    void
    InitializePhysx( )
    {
        // init physx
        m_Foundation = PxCreateFoundation( PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback );
        if ( !m_Foundation ) throw std::runtime_error( "PxCreateFoundation failed!" );

        m_Pvd                            = PxCreatePvd( *m_Foundation );
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
        m_Pvd->connect( *transport, physx::PxPvdInstrumentationFlag::eALL );

        m_ToleranceScale.length = 1;      // typical length of an object
        m_ToleranceScale.speed  = 9.81;   // typical speed of an object, gravity*1s is a reasonable choice
        m_Physics               = PxCreatePhysics( PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_Pvd );
        // mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

        m_MainScene = std::make_shared<PhysicsScene>( m_Physics );
        m_Cooking   = PxCreateCooking( PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams( m_ToleranceScale ) );
        if ( !m_Cooking ) throw std::runtime_error( "PxCreateCooking failed!" );
    }

public:
    PhysicsEngine( )
    {
        InitializePhysx( );
    }

    ~PhysicsEngine( )
    {
        m_Physics->release( );
        m_Cooking->release( );
        m_Foundation->release( );

        spdlog::info( "PhysX released!" );
    }

    auto*
    GetPhysxHandle( ) { return m_Physics; }

    auto&
    GetScene( ) { return *m_MainScene; }

    auto&
    GetCooking( ) { return *m_Cooking; }

    operator physx::PxPhysics&( ) { return *m_Physics; }
    auto* operator->( ) { return GetPhysxHandle( ); }

private:
    physx::PxDefaultAllocator     m_DefaultAllocatorCallback;
    physx::PxDefaultErrorCallback m_DefaultErrorCallback;
    physx::PxTolerancesScale      m_ToleranceScale;

    physx::PxCooking*    m_Cooking    = nullptr;
    physx::PxFoundation* m_Foundation = nullptr;
    physx::PxPhysics*    m_Physics    = nullptr;

    std::shared_ptr<PhysicsScene> m_MainScene;

    physx::PxPvd* m_Pvd = nullptr;
};

template <typename Span>
concept SpanLike = requires( Span span ) {
    {
        span.size( )
    } -> std::convertible_to<std::size_t>;
    {
        span.data( )
    };
};

class GroupMeshColliderSerializer
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
        PhysicsMeshInstance( std::pair<std::ptrdiff_t, size_t> BufferSpan, ColliderType Type, physx::PxBase* PxMesh = nullptr )
            : BufferSpan( BufferSpan )
            , Type( Type )
            , PxMesh( PxMesh )
        { }
        ~PhysicsMeshInstance( )
        {
            if ( PxMesh != nullptr )
            {
                PxMesh->release( );
                PxMesh = nullptr;
            }
        }

        PhysicsMeshInstance( PhysicsMeshInstance&& Other ) { *this = std::move( Other ); }
        PhysicsMeshInstance& operator=( PhysicsMeshInstance&& Other )
        {
            BufferSpan = std::move( Other.BufferSpan );
            if ( PxMesh != nullptr )
            {
                PxMesh->release( );
                PxMesh = nullptr;
            }

            Type = Other.Type;
            std::swap( PxMesh, Other.PxMesh );

            if ( PxMesh != nullptr )
            {
                switch ( Type )
                {
                case ColliderType::eNone: break;
                case ColliderType::eConvex:
                    spdlog::info( "Ref Count: {}", ( (physx::PxConvexMesh*) PxMesh )->getReferenceCount( ) );
                    break;
                case ColliderType::eTriangle:
                    spdlog::info( "Ref Count: {}", ( (physx::PxTriangleMesh*) PxMesh )->getReferenceCount( ) );
                    break;
                }
            }

            return *this;
        }
        PhysicsMeshInstance( const PhysicsMeshInstance& )            = delete;   // non construction-copyable
        PhysicsMeshInstance& operator=( const PhysicsMeshInstance& ) = delete;   // non copyable

        [[nodiscard]] physx::PxGeometryHolder
        GenerateGeometry( ) const
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
        GenerateMeshIfNull( PhysicsEngine* PhyEngine, uint8_t* Data )
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
    };

    void
    PushCookedBuffer( physx::PxDefaultMemoryOutputStream& CookingBuffer, ColliderType CType )
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

public:
    /*
     *
     * This will not load or generate any data
     *
     * */
    GroupMeshColliderSerializer( std::string Name, PhysicsEngine* PhyEngine )
        : m_GroupName( std::move( Name ) )
        , m_PhyEngine( PhyEngine )
    { }

    /*
     *
     * This will load the cached data from file or, generate it and write to file
     *
     * */
    template <typename Mapping = void*>
    GroupMeshColliderSerializer( const ConceptMesh& Mesh, PhysicsEngine* PhyEngine, Mapping&& ColliderMapping = nullptr )
        : m_GroupName( GetHashFilePath( Mesh.GetFilePath( ) ) )
        , m_PhyEngine( PhyEngine )
    {
        auto& SubMeshes  = Mesh.GetSubMeshes( );
        bool  ShouldLoad = !TryLoad( ) || !IsColliderTypeCorrect( SubMeshes, ColliderMapping );
        if ( ShouldLoad )
        {
            spdlog::info( "Number of ModelSubMeshSpan: {}", SubMeshes.size( ) );
            GenerateFrom( SubMeshes, ColliderMapping );
            WriteCacheToFile( );
        }
    }

    static std::string
    GetHashFilePath( std::string_view FilePath )
    {
        return std::to_string( hash_value( std::filesystem::path( FilePath ) ) );
    }

    std::filesystem::path
    GetFilePath( )
    {
        return PATH_PREFIX / ( m_GroupName + "_group_mesh_cache.bin" );
    }

    template <typename Ty>
    void
    BufferReadValue( auto*& Buffer, Ty& Value )
    {
        Value = *reinterpret_cast<Ty*>( Buffer );

        static_assert( sizeof( Buffer[ 0 ] ) == sizeof( char ) );
        Buffer += sizeof( Ty );
    }

    /*
     *
     * Try loading from offline saved data
     *
     * */
    bool TryLoad( )
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

    bool IsColliderTypeCorrect( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping )
    {
        // Check if mapping is the same
        if ( ModelSubMeshSpan.size( ) != m_PxMeshInstances.size( ) )
            return false;

        for ( int i = 0; i < ModelSubMeshSpan.size( ); ++i )
        {
            if constexpr ( requires { { ColliderMapping( ModelSubMeshSpan[ i ] ) } -> std::convertible_to<GroupMeshColliderSerializer::ColliderType>; } )
            {
                GroupMeshColliderSerializer::ColliderType Type = ColliderMapping( ModelSubMeshSpan[ i ] );

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
    GenerateFrom( const std::vector<SubMeshSpan>& ModelSubMeshSpan, auto&& ColliderMapping )
    {
        ClearBuffer( );
        for ( auto& SubMeshSpan : ModelSubMeshSpan )
        {
            try
            {
                if constexpr ( requires { { ColliderMapping( SubMeshSpan ) } -> std::convertible_to<GroupMeshColliderSerializer::ColliderType>; } )
                {
                    GroupMeshColliderSerializer::ColliderType Type = ColliderMapping( SubMeshSpan );
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

    template <SpanLike Span>
    void
    AppendConvexCache( Span& VertexBuffer, uint16_t ConvexVertexLimit = 100 )
    {
        physx::PxConvexMeshDesc convexDesc;
        convexDesc.points.count  = VertexBuffer.size( );
        convexDesc.points.stride = sizeof( VertexBuffer[ 0 ] );
        convexDesc.points.data   = VertexBuffer.data( );
        convexDesc.flags         = physx::PxConvexFlag::eCOMPUTE_CONVEX
            | physx::PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES
            | physx::PxConvexFlag::eSHIFT_VERTICES;   // Need to verify
        convexDesc.vertexLimit = ConvexVertexLimit;

        physx::PxDefaultMemoryOutputStream     CookingBuffer;
        physx::PxConvexMeshCookingResult::Enum result;

        auto& PhysxCooking = m_PhyEngine->GetCooking( );
        auto  TriParams = PhysxCooking.getParams( ), ParamsCopy = TriParams;

        TriParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
        TriParams.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
        m_PhyEngine->GetCooking( ).setParams( TriParams );

        if ( !m_PhyEngine->GetCooking( ).cookConvexMesh( convexDesc, CookingBuffer, &result ) )
        {
            m_PhyEngine->GetCooking( ).setParams( ParamsCopy );
            throw std::runtime_error( "Failed to cook convex mesh" );
        }

        m_PhyEngine->GetCooking( ).setParams( ParamsCopy );
        PushCookedBuffer( CookingBuffer, ColliderType::eConvex );
    }

    template <SpanLike VertexSpan, SpanLike IndexSpan>
    void
    AppendTriangleCache( VertexSpan& VertexBuffer, IndexSpan& IndexBuffer )
    {
        REQUIRED( IndexBuffer.size( ) % 3 == 0, return; )

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count  = VertexBuffer.size( );
        meshDesc.points.stride = sizeof( VertexBuffer[ 0 ] );
        meshDesc.points.data   = VertexBuffer.data( );

        meshDesc.triangles.count  = IndexBuffer.size( ) / 3;
        meshDesc.triangles.stride = 3 * sizeof( IndexBuffer[ 0 ] );
        meshDesc.triangles.data   = IndexBuffer.data( );

        physx::PxDefaultMemoryOutputStream       CookingBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        if ( !m_PhyEngine->GetCooking( ).cookTriangleMesh( meshDesc, CookingBuffer, &result ) )
            throw std::runtime_error( "Failed to cook triangle mesh" );

        PushCookedBuffer( CookingBuffer, ColliderType::eTriangle );
    }

    physx::PxRigidDynamic*
    CreateDynamicRigidBodyFromCacheGroup( const physx::PxMaterial& material )
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
    CreateStaticRigidBodyFromCacheGroup( const physx::PxMaterial& material )
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

    void WriteCacheToFile( )
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
    ClearBuffer( )
    {
        m_PxMeshInstances.clear( );
        m_CacheBuffer.clear( );
    }

    bool
    HasData( )
    {
        REQUIRED( m_CacheBuffer.empty( ) == m_PxMeshInstances.empty( ) )
        return !m_CacheBuffer.empty( );
    }

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

// FIXME: Use PureConcept
class Collider : public Concept
{
    DECLARE_CONCEPT( Collider, Concept )

public:
    Collider( PhysicsEngine* PhyEngine )
        : m_PhyEngine( PhyEngine )
    { }

    physx::PxRigidActor*
    GetRigidBodyHandle( )
    {
        return m_RigidActor;
    }

protected:
    /*
     *
     * Physx
     *
     * */
    PhysicsEngine*       m_PhyEngine  = nullptr;
    physx::PxRigidActor* m_RigidActor = nullptr;
};
DEFINE_CONCEPT( Collider )
Collider::~Collider( )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }
}
/*
 *
 * Collider got meshes, should not be shared
 *
 * */
class ColliderMesh : public Collider
{
    DECLARE_CONCEPT( ColliderMesh, Collider )

public:
    /*
     *
     * This will generate a new collider for the mesh
     *
     * */
    template <typename Mapping = void*>
    ColliderMesh( std::shared_ptr<ConceptMesh> StaticMesh, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
        : Collider( PhyEngine )
    {
        SetGroupMeshCollider( std::make_shared<GroupMeshColliderSerializer>( *StaticMesh, m_PhyEngine, ColliderMapping ), Material, Static );
    }

    ColliderMesh( std::shared_ptr<GroupMeshColliderSerializer> GroupMeshCollider, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false )
        : Collider( PhyEngine )
    {
        SetGroupMeshCollider( GroupMeshCollider, Material, Static );
    }

    /*
     *
     * GroupMeshCollider need to be loaded and initialized
     *
     * */
    void
    SetGroupMeshCollider( std::shared_ptr<GroupMeshColliderSerializer> GroupMeshCollider, physx::PxMaterial* Material, bool Static )
    {
        m_GroupMeshCollider = GroupMeshCollider;
        REQUIRED( m_GroupMeshCollider->HasData( ), throw std::runtime_error( "GroupMeshCollider has no data" ) )

        if ( m_RigidActor != nullptr )
        {
            m_RigidActor->release( );
            m_RigidActor = nullptr;
        }

        if ( Static )
            m_RigidActor = m_GroupMeshCollider->CreateStaticRigidBodyFromCacheGroup( *Material );
        else
            m_RigidActor = m_GroupMeshCollider->CreateDynamicRigidBodyFromCacheGroup( *Material );

        REQUIRED( this == dynamic_cast<Collider*>( this ) )
        // FIXME: should set by rigid body
        m_RigidActor->userData = dynamic_cast<Collider*>( this );
        m_PhyEngine->GetScene( )->addActor( *m_RigidActor );
    }

private:
    /*
     *
     * Collider
     *
     * */
    std::shared_ptr<GroupMeshColliderSerializer> m_GroupMeshCollider;
};
DEFINE_CONCEPT_DS( ColliderMesh )

class RigidBody : public ConceptApplicable
    , protected Orientation
{
    DECLARE_CONCEPT( RigidBody, ConceptApplicable )
public:
    RigidBody( )
    {
        // For collider or renderable etc.
        SetSearchThrough( true );
    }

    void
    Apply( ) override
    {
        // Make sure to release unlinked references
        GetConcepts( m_RenderableConcepts );

        if ( OrientationChanged )
        {
            m_RenderableConcepts.ForEach( [ this ]( std::shared_ptr<ConceptRenderable>& Renderable ) {
                Renderable->SetShaderUniform( "modelMatrix", GetModelMatrix( ) );
            } );
            OrientationChanged = false;
        }
    }

    Orientation&
    GetOrientation( )
    {
        OrientationChanged = true;
        return *static_cast<Orientation*>( this );
    }

protected:
    bool                                    OrientationChanged = true;
    ConceptSetFetchCache<ConceptRenderable> m_RenderableConcepts;
};
DEFINE_CONCEPT_DS( RigidBody )

/*
 *
 * Contain renderable mesh and collider
 *
 * */
class RigidMesh : public RigidBody
{
    DECLARE_CONCEPT( RigidMesh, RigidBody )

public:
    template <typename Mapping = void*>
    RigidMesh( const std::string& MeshPathStr, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false, Mapping&& ColliderMapping = nullptr )
    {
        REQUIRED( !MeshPathStr.empty( ), throw std::runtime_error( "Mesh path cannot be empty" ); )

        SerializerModel Serializer;

        auto Mesh = CreateConcept<ConceptMesh>( );
        Serializer.ToMesh( MeshPathStr, Mesh.get( ) );

        SetMesh( Mesh );

        REQUIRED( PhyEngine != nullptr && Material != nullptr, throw std::runtime_error( "Physx engine and material cannot be nullptr" ); )
        SetCollider( CreateConcept<ColliderMesh>( Mesh, PhyEngine, Material, Static, ColliderMapping ) );
    }

    RigidMesh( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<Collider> C )
    {
        SetMesh( std::move( Mesh ) );
        SetCollider( std::move( C ) );
    }

    void
    SetMesh( std::shared_ptr<ConceptMesh> Mesh )
    {
        auto* Camera     = PureConceptCamera::PeekCameraStack<PureConceptPerspectiveCamera>( );
        m_RenderableMesh = AddConcept<RenderableMesh>( Mesh );
        m_RenderableMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
        m_RenderableMesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
        m_RenderableMesh->SetShaderUniform( "viewPos", Camera->GetCameraPosition( ) );
        m_RenderableMesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
    }

    void
    SetCollider( std::shared_ptr<Collider> C )
    {
        if ( m_Collider != nullptr ) m_Collider->Destroy( );
        REQUIRED( GetOwnership( m_Collider = C ) )
    }

    [[nodiscard]] const auto&
    GetCollider( ) const noexcept { return m_Collider; }

protected:
    std::shared_ptr<RenderableMesh> m_RenderableMesh;
    std::shared_ptr<Collider>       m_Collider;
};
DEFINE_CONCEPT_DS( RigidMesh )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    constexpr std::pair<int, int> WindowSize = { 1920, 1080 };
    Engine::GetEngine( )->SetLogicalMainWindowViewPortDimensions( WindowSize );

    {
        m_MainCamera = AddConcept<PureConceptPerspectiveCamera>( );
        m_MainCamera->SetRuntimeName( "Main Camera" );
        m_MainCamera->PushToCameraStack( );

        m_MainCamera->SetCameraPerspectiveFOV( 45, false );
        m_MainCamera->SetCameraPosition( glm::vec3( 0.0f, 3.0f, 3.0f ), false );
        m_MainCamera->SetCameraFacing( glm::vec3( 0.0f, -0.70710678118F, -0.70710678118F ), false );
    }

    {
        const auto& PerspectiveCanvas = AddConcept<Canvas>( );
        PerspectiveCanvas->SetRuntimeName( "Perspective Canvas" );
        PerspectiveCanvas->SetCanvasCamera( m_MainCamera );

        {
            auto StaticMesh = AddConcept<ConceptMesh>( );
            StaticMesh->DetachFromOwner( );

            {
                SerializerModel TestModel;
                TestModel.SetFilePath( "Assets/Model/red_cube.glb" );
                TestModel.ToMesh( StaticMesh.get( ) );
            }

            auto LightMesh = PerspectiveCanvas->AddConcept<RenderableMesh>( StaticMesh );
            LightMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );

            AddConcept<CameraController>( m_MainCamera );

            if ( true )
            {
                m_PhyEngine = std::make_shared<PhysicsEngine>( );

                physx::PxMaterial*    Material    = ( *m_PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *m_PhyEngine, physx::PxPlane( 0, 1, 0, 3 ), *Material );
                m_PhyEngine->GetScene( )->addActor( *groundPlane );

                auto RM = AddConcept<RigidMesh>( "Assets/Model/low_poly_room.glb", m_PhyEngine.get( ), Material, true, []( auto& SubMeshSpan ) {
                    if ( SubMeshSpan.SubMeshName.find( "Wall" ) != std::string::npos )
                        return GroupMeshColliderSerializer::ColliderType::eTriangle;
                    else
                        return GroupMeshColliderSerializer::ColliderType::eConvex;
                } );
                AddConcept<LightRotate>( RM->GetConcept<RenderableMesh>( ), LightMesh );

                {
                    SerializerModel Serializer;
                    auto            Mesh = CreateConcept<ConceptMesh>( );
                    Serializer.ToMesh( "Assets/Model/red_cube.glb", Mesh.get( ) );
                    auto MeshColliderData = std::make_shared<GroupMeshColliderSerializer>( *StaticMesh, m_PhyEngine.get( ) );

                    for ( int i = 0; i < 10; ++i )
                    {
                        auto* RBH = AddConcept<RigidMesh>( Mesh, CreateConcept<ColliderMesh>( MeshColliderData, m_PhyEngine.get( ), Material ) )->GetCollider( )->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                        REQUIRED_IF( RBH != nullptr )
                        {
                            RBH->setGlobalPose( physx::PxTransform { physx::PxVec3( 0, 15 + i * 3, 0 ) } );
                            RBH->setAngularVelocity( { (float) i, (float) 20, (float) -10 }, true );
                        }
                    }
                }
            }
        }
    }

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    constexpr int DesieredFPS = 160;
    constexpr int CapFPS      = DesieredFPS * 2;

    const auto DeltaSecond = Engine::GetEngine( )->GetDeltaSecond( );

    {
        m_PhyEngine->GetScene( )->simulate( DeltaSecond );

        float         WaitTimeMilliseconds      = 500.F / DesieredFPS;
        constexpr int MaxWaitTimeMilliseconds   = 1000;
        int           TotalWaitTimeMilliseconds = 0;
        while ( !m_PhyEngine->GetScene( )->fetchResults( false ) )
        {
            const auto SleepTime = std::min( MaxWaitTimeMilliseconds, int( WaitTimeMilliseconds ) );
            TotalWaitTimeMilliseconds += SleepTime;
            spdlog::info( "Wait fetchResults for {}ms, acc: {}ms.", SleepTime, TotalWaitTimeMilliseconds );
            std::this_thread::sleep_for( std::chrono::milliseconds( SleepTime ) );
            WaitTimeMilliseconds = SleepTime * 1.1F;
        }

        const auto SleepTime = 1000.F / CapFPS - DeltaSecond * 1000;
        if ( SleepTime > TotalWaitTimeMilliseconds )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( int( SleepTime ) - TotalWaitTimeMilliseconds ) );
            spdlog::info( "Sleep for stable framerate: {}ms.", int( SleepTime ) - TotalWaitTimeMilliseconds );
        }
    }

    // retrieve array of actors that moved
    physx::PxU32     nbActiveActors;
    physx::PxActor** activeActors = m_PhyEngine->GetScene( )->getActiveActors( nbActiveActors );

    if ( nbActiveActors != 0 )
    {
        // update each render object with the new transform
        for ( physx::PxU32 i = 0; i < nbActiveActors; ++i )
        {
            auto* Data = activeActors[ i ]->userData;
            if ( Data != nullptr )
            {
                auto* ColliderMeshPtr = static_cast<Collider*>( Data );

                auto* ColliderOwnerPtr = ColliderMeshPtr->GetOwner( );
                if ( auto RB = ColliderOwnerPtr->TryCast<RigidBody>( ); RB != nullptr )
                {
                    const auto& GP = ( (physx::PxRigidActor*) activeActors[ i ] )->getGlobalPose( );
                    RB->GetOrientation( ).SetCoordinate( GP.p.x, GP.p.y, GP.p.z );

                    static_assert( sizeof( physx::PxQuat ) == sizeof( glm::quat ) );
                    RB->GetOrientation( ).SetQuat( *( (glm::quat*) &GP.q ) );
                }
            }
        }
    }
}

void
GameManager::TestInvokable( )
{
    spdlog::info( "GameManager::TestInvokable" );

    if ( GetConcept<WandEditorScene>( ) )
    {
        if ( m_WandEditorScene != nullptr )
        {
            m_WandEditorScene->Destroy( );
        }
    } else
    {
        if ( m_WandEditorScene != nullptr )
        {
            GetOwnership( m_WandEditorScene );
        } else
        {
            m_WandEditorScene = AddConcept<WandEditorScene>( );
        }
    }
}