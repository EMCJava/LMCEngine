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

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, ConceptApplicable, TestInvokable )

class LightRotate : public ConceptApplicable
{
    DECLARE_CONCEPT( LightRotate, ConceptApplicable )
public:
    LightRotate( std::shared_ptr<RenderableMesh> Mesh, std::shared_ptr<RenderableMesh> LightMesh )
        : m_Mesh( std::move( Mesh ) )
        , m_LightMesh( std::move( LightMesh ) )
    {
        m_LightMesh->SetScale( 0.08, 0.08, 0.08, true );
    }

    void
    Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_AccumulatedTime += DeltaTime;

        const auto Location = glm::vec3( cos( m_AccumulatedTime * 2 ) * 5.0f, 2, sin( m_AccumulatedTime * 2 ) * 5.0f );
        m_Mesh->SetShaderUniform( "lightPos", Location );
        m_LightMesh->SetCoordinate( Location, true );
    }

protected:
    FloatTy                         m_AccumulatedTime { 0.0f };
    std::shared_ptr<RenderableMesh> m_Mesh;
    std::shared_ptr<RenderableMesh> m_LightMesh;
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

        m_ToleranceScale.length = 1;     // typical length of an object
        m_ToleranceScale.speed  = 981;   // typical speed of an object, gravity*1s is a reasonable choice
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
class GroupMeshHitBoxSerializer
{
public:
    enum class HitBoxType : uint8_t {
        eNone     = 0,
        eConvex   = 1,
        eTriangle = 2
    };

private:
    void
    PushCookedBuffer( physx::PxDefaultMemoryOutputStream& CookingBuffer, HitBoxType Type )
    {
        const auto OldSize = m_CacheBuffer.size( );
        m_CacheBuffer.resize( OldSize + sizeof( size_t ) + sizeof( HitBoxType ) + CookingBuffer.getSize( ) );

        char* ChunkStart = m_CacheBuffer.data( ) + OldSize;

        // Buffer size
        size_t BufferSize = CookingBuffer.getSize( );
        memcpy( ChunkStart, &BufferSize, sizeof( size_t ) );
        // Buffer type
        *reinterpret_cast<HitBoxType*>( ChunkStart + sizeof( size_t ) ) = Type;
        // Buffer data
        memcpy( ChunkStart + sizeof( size_t ) + sizeof( HitBoxType ), CookingBuffer.getData( ), CookingBuffer.getSize( ) );

        // Skip list save
        m_CacheBufferChunk.emplace_back( OldSize + sizeof( size_t ) + sizeof( HitBoxType ), CookingBuffer.getSize( ) );

        spdlog::info( "Cooked buffer size: {}[{}]", CookingBuffer.getSize( ), (uint8_t) Type );
    }

public:
    GroupMeshHitBoxSerializer( std::string Name, PhysicsEngine* PhyEngine )
        : m_GroupName( std::move( Name ) )
        , m_PhyEngine( PhyEngine )
    { }

    std::filesystem::path
    GetFilePath( )
    {
        return PATH_PREFIX / ( m_GroupName + "_group_mesh_cache.bin" );
    }

    template <typename Ty>
    void
    BufferReadValue( char*& Buffer, Ty& Value )
    {
        Value = *reinterpret_cast<Ty*>( Buffer );
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

            m_CacheBufferChunk.clear( );
            m_CacheBuffer.resize( BufferSize );
            InputStream.read( m_CacheBuffer.data( ), BufferSize );
            if ( BufferSize != InputStream.gcount( ) ) throw std::runtime_error( "Failed to read convex.bin" );

            char* BufferBegin = m_CacheBuffer.data( );
            char* BufferEnd   = BufferBegin + BufferSize;

            while ( BufferBegin != BufferEnd )
            {
                size_t     SpanSize;
                HitBoxType Type;

                BufferReadValue( BufferBegin, SpanSize );
                BufferReadValue( BufferBegin, Type );

                m_CacheBufferChunk.emplace_back( std::distance( m_CacheBuffer.data( ), BufferBegin ), SpanSize );
                BufferBegin += SpanSize;
            }

            return true;
        }

        return false;
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
        PushCookedBuffer( CookingBuffer, HitBoxType::eConvex );
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

        PushCookedBuffer( CookingBuffer, HitBoxType::eTriangle );
    }

    physx::PxRigidDynamic*
    CreateDynamicRigidBodyFromCacheGroup( const physx::PxMaterial& material )
    {
        auto* hfActor = ( *m_PhyEngine )->createRigidDynamic( physx::PxTransform( physx::PxVec3( 0 ) ) );
        hfActor->setName( m_GroupName.c_str( ) );

        for ( auto& Chunk : m_CacheBufferChunk )
        {
            physx::PxDefaultMemoryInputData input( (uint8_t*) m_CacheBuffer.data( ) + Chunk.first, Chunk.second );
            switch ( *reinterpret_cast<HitBoxType*>( m_CacheBuffer.data( ) + Chunk.first - sizeof( HitBoxType ) ) )
            {
            case HitBoxType::eNone: break;
            case HitBoxType::eConvex: {
                physx::PxConvexMesh* convexMesh   = ( *m_PhyEngine )->createConvexMesh( input );
                physx::PxShape*      aConvexShape = physx::PxRigidActorExt::createExclusiveShape( *hfActor, physx::PxConvexMeshGeometry( convexMesh ), material );
                REQUIRED( aConvexShape != nullptr )
                break;
            }
            case HitBoxType::eTriangle:
                spdlog::warn( "Skipping triangle mesh for dynamic rigid body(not supported by Physx)." );
                break;
            }
        }

        return hfActor;
    }

    physx::PxRigidStatic*
    CreateStaticRigidBodyFromCacheGroup( const physx::PxMaterial& material )
    {
        auto* hfActor = ( *m_PhyEngine )->createRigidStatic( physx::PxTransform( physx::PxVec3( 0 ) ) );
        hfActor->setName( m_GroupName.c_str( ) );

        for ( auto& Chunk : m_CacheBufferChunk )
        {
            physx::PxDefaultMemoryInputData input( (uint8_t*) m_CacheBuffer.data( ) + Chunk.first, Chunk.second );
            switch ( *reinterpret_cast<HitBoxType*>( m_CacheBuffer.data( ) + Chunk.first - sizeof( HitBoxType ) ) )
            {
            case HitBoxType::eNone: break;
            case HitBoxType::eConvex: {
                physx::PxConvexMesh* convexMesh   = ( *m_PhyEngine )->createConvexMesh( input );
                physx::PxShape*      aConvexShape = physx::PxRigidActorExt::createExclusiveShape( *hfActor, physx::PxConvexMeshGeometry( convexMesh ), material );
                REQUIRED( aConvexShape != nullptr )
                break;
            }
            case HitBoxType::eTriangle: {
                physx::PxTriangleMesh* triangleMesh = ( *m_PhyEngine )->createTriangleMesh( input );
                physx::PxShape*        aConvexShape = physx::PxRigidActorExt::createExclusiveShape( *hfActor, physx::PxTriangleMeshGeometry( triangleMesh ), material );
                REQUIRED( aConvexShape != nullptr )
                break;
            }
            }
        }

        return hfActor;
    }

    void WriteCache( )
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
        if ( !OutputStream.write( m_CacheBuffer.data( ), m_CacheBuffer.size( ) ) )
        {
            throw std::runtime_error( "Failed to write convex cache" );
        }
    }

    const auto&
    GetCacheBuffer( ) const { return m_CacheBuffer; }

    const auto&
    GetCacheBufferChunk( ) const { return m_CacheBufferChunk; }

private:
    const std::filesystem::path PATH_PREFIX = "Assets/Physics/Cache/Convexes/";
    std::string                 m_GroupName { };

    // [buffer size(size_t), buffer type(HitBoxType), buffer], ...
    std::vector<char>                              m_CacheBuffer;
    std::vector<std::pair<std::ptrdiff_t, size_t>> m_CacheBufferChunk { };

    /*
     *
     * Physx
     *
     * */
    PhysicsEngine* m_PhyEngine = nullptr;
};

class StaticMesh : public PureConcept
{
    DECLARE_CONCEPT( StaticMesh, PureConcept )

public:
    template <typename Mapping = void*>
    explicit StaticMesh( const std::string& MeshPathStr = "", bool Static = false, Mapping&& HitBoxMapping = nullptr )
    {
    }
};
DEFINE_CONCEPT( StaticMesh );

class RigidMesh : public ConceptApplicable
{
    DECLARE_CONCEPT( RigidMesh, ConceptApplicable )

public:
    template <typename Mapping = void*>
    RigidMesh( const std::string& MeshPathStr, PhysicsEngine* PhyEngine, physx::PxMaterial* Material, bool Static = false, Mapping&& HitBoxMapping = nullptr )
        : m_PhyEngine( PhyEngine )
    {

        SerializerModel Model;
        Model.SetFilePath( MeshPathStr );

        auto StaticMesh = AddConcept<ConceptMesh>( );
        StaticMesh->DetachFromOwner( );

        std::vector<SubMeshSpan> ModelSubMeshSpan;
        Model.ToMesh( StaticMesh.get( ), &ModelSubMeshSpan );

        auto* Camera = PureConceptCamera::PeekCameraStack<PureConceptPerspectiveCamera>( );
        auto  Mesh   = AddConcept<RenderableMesh>( StaticMesh );
        Mesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
        Mesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
        Mesh->SetShaderUniform( "viewPos", Camera->GetCameraPosition( ) );
        Mesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );

        const auto MeshPathHash = std::to_string( hash_value( std::filesystem::path( MeshPathStr ) ) );

        constexpr GroupMeshHitBoxSerializer::HitBoxType DefaultHitBoxType = GroupMeshHitBoxSerializer::HitBoxType::eConvex;
        GroupMeshHitBoxSerializer                       GCS( MeshPathHash, m_PhyEngine );
        bool                                            ShouldLoad = !GCS.TryLoad( );
        if ( !ShouldLoad )
        {
            // Check if mapping is the same
            if ( ModelSubMeshSpan.size( ) != GCS.GetCacheBufferChunk( ).size( ) )
                ShouldLoad = true;
            else
            {
                for ( int i = 0; i < ModelSubMeshSpan.size( ); ++i )
                {
                    const auto CacheType = *reinterpret_cast<const GroupMeshHitBoxSerializer::HitBoxType*>( GCS.GetCacheBuffer( ).data( ) + GCS.GetCacheBufferChunk( )[ i ].first - sizeof( GroupMeshHitBoxSerializer::HitBoxType ) );
                    if constexpr ( requires { { HitBoxMapping( ModelSubMeshSpan[ i ] ) } -> std::convertible_to<GroupMeshHitBoxSerializer::HitBoxType>; } )
                    {
                        GroupMeshHitBoxSerializer::HitBoxType Type = HitBoxMapping( ModelSubMeshSpan[ i ] );

                        if ( Type != CacheType )
                        {
                            ShouldLoad = true;
                            break;
                        }
                    } else
                    {
                        if ( DefaultHitBoxType != CacheType )
                        {
                            ShouldLoad = true;
                            break;
                        }
                    }
                }
            }
        }

        if ( ShouldLoad )
        {
            spdlog::info( "Number of ModelSubMeshSpan: {}", ModelSubMeshSpan.size( ) );

            for ( auto& SubMeshSpan : ModelSubMeshSpan )
            {
                try
                {
                    if constexpr ( requires { { HitBoxMapping( SubMeshSpan ) } -> std::convertible_to<GroupMeshHitBoxSerializer::HitBoxType>; } )
                    {
                        GroupMeshHitBoxSerializer::HitBoxType Type = HitBoxMapping( SubMeshSpan );
                        switch ( Type )
                        {
                        case GroupMeshHitBoxSerializer::HitBoxType::eNone: break;
                        case GroupMeshHitBoxSerializer::HitBoxType::eConvex:
                            GCS.AppendConvexCache( SubMeshSpan.VertexRange );
                            break;
                        case GroupMeshHitBoxSerializer::HitBoxType::eTriangle:
                            GCS.AppendTriangleCache( SubMeshSpan.VertexRange, SubMeshSpan.RebasedIndexRange );
                            break;
                        }
                    } else
                    {
                        spdlog::warn( "Using convex mesh by default" );
                        GCS.AppendConvexCache( SubMeshSpan.VertexRange );
                    }
                }
                catch ( std::runtime_error& e )
                {
                    spdlog::error( "Failed to load model: {}", e.what( ) );
                }
            }

            GCS.WriteCache( );
        }

        if ( m_RigidActor != nullptr )
        {
            m_RigidActor->release( );
        }

        if ( Static )
            m_RigidActor = GCS.CreateStaticRigidBodyFromCacheGroup( *Material );
        else
            m_RigidActor = GCS.CreateDynamicRigidBodyFromCacheGroup( *Material );

        m_RigidActor->userData = this;
        m_PhyEngine->GetScene( )->addActor( *m_RigidActor );

        SetRuntimeName( "RigidCube LOL" );
        SetSearchThrough( true );
    }

    physx::PxRigidActor*
    GetRigidBodyHandle( )
    {
        return m_RigidActor;
    }

    void
    Apply( ) override { }

private:
    /*
     *
     * Physx
     *
     * */
    PhysicsEngine* m_PhyEngine = nullptr;

    physx::PxRigidActor* m_RigidActor = nullptr;
};
DEFINE_CONCEPT( RigidMesh )
RigidMesh::~RigidMesh( )
{
    if ( m_RigidActor != nullptr )
    {
        m_RigidActor->release( );
        m_RigidActor = nullptr;
    }
}


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

            SerializerModel TestModel;
            TestModel.SetFilePath( "Assets/Model/red_cube.glb" );
            TestModel.ToMesh( StaticMesh.get( ) );

            auto LightMesh = PerspectiveCanvas->AddConcept<RenderableMesh>( StaticMesh );
            LightMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );

            AddConcept<CameraController>( m_MainCamera );

            if ( true )
            {
                m_PhyEngine = std::make_shared<PhysicsEngine>( );

                physx::PxMaterial*    Material    = ( *m_PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *m_PhyEngine, physx::PxPlane( 0, 1, 0, 50 ), *Material );
                m_PhyEngine->GetScene( )->addActor( *groundPlane );

                auto RM = AddConcept<RigidMesh>( "Assets/Model/low_poly_room.glb", m_PhyEngine.get( ), Material, true, []( auto& SubMeshSpan ) {
                    if ( SubMeshSpan.SubMeshName.find( "Wall" ) != std::string::npos )
                        return GroupMeshHitBoxSerializer::HitBoxType::eTriangle;
                    else
                        return GroupMeshHitBoxSerializer::HitBoxType::eConvex;
                } );
                AddConcept<LightRotate>( RM->GetConcept<RenderableMesh>( ), LightMesh );

                for ( int i = 0; i < 10; ++i )
                {
                    auto* RBH = AddConcept<RigidMesh>( "Assets/Model/red_cube.glb", m_PhyEngine.get( ), Material )->GetRigidBodyHandle( )->is<physx::PxRigidBody>( );
                    REQUIRED_IF( RBH != nullptr )
                    {
                        RBH->setGlobalPose( physx::PxTransform { physx::PxVec3( 0, 15 + i * 3, 0 ) } );
                        RBH->setAngularVelocity( { (float) i, (float) 20, (float) -10 }, true );
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
    {
        m_PhyEngine->GetScene( )->simulate( 1 / 60.f );
        spdlog::info( "Start fetchResults" );

        float         WaitTimeMilliseconds      = 1000.F / 60;
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
        spdlog::info( "End fetchResults, time used: {}ms.", TotalWaitTimeMilliseconds );

        const auto SleepTime = 1000.F / 60 - Engine::GetEngine( )->GetDeltaSecond( ) * 1000;
        if ( SleepTime > TotalWaitTimeMilliseconds )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( int( SleepTime ) - TotalWaitTimeMilliseconds ) );
            spdlog::info( "Sleep for stable framerate: {}ms.", int( SleepTime ) - TotalWaitTimeMilliseconds );
        }
    }

    // retrieve array of actors that moved
    physx::PxU32     nbActiveActors;
    physx::PxActor** activeActors = m_PhyEngine->GetScene( )->getActiveActors( nbActiveActors );
    spdlog::info( "nbActiveActors: {}", nbActiveActors );

    if ( nbActiveActors != 0 )
    {
        // update each render object with the new transform
        for ( physx::PxU32 i = 0; i < nbActiveActors; ++i )
        {
            auto* Data = activeActors[ i ]->userData;
            if ( Data != nullptr )
            {
                auto* RigidMeshPtr = static_cast<RigidMesh*>( Data );
                spdlog::info( "{} is moving", RigidMeshPtr->GetRuntimeName( ) );

                if ( auto Mesh = RigidMeshPtr->GetConcept<RenderableMesh>( ); Mesh != nullptr )
                {
                    const auto& GP = ( (physx::PxRigidActor*) activeActors[ i ] )->getGlobalPose( );
                    Mesh->SetCoordinate( GP.p.x, GP.p.y, GP.p.z );

                    const auto& q     = GP.q;
                    float       yaw   = atan2( 2.0 * ( q.y * q.z + q.w * q.x ), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z );
                    float       pitch = asin( -2.0 * ( q.x * q.z - q.w * q.y ) );
                    float       roll  = atan2( 2.0 * ( q.x * q.y + q.w * q.z ), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z );

                    Mesh->SetRotation( yaw, pitch, roll );
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