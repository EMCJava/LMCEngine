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
    LightRotate( std::shared_ptr<ConceptMesh> Mesh, std::shared_ptr<ConceptMesh> LightMesh )
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
    FloatTy                      m_AccumulatedTime { 0.0f };
    std::shared_ptr<ConceptMesh> m_Mesh;
    std::shared_ptr<ConceptMesh> m_LightMesh;
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

        if ( UserInputHandle->GetKeyState( GLFW_KEY_W ).isDown ) FrontMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_S ).isDown ) FrontMovement -= 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_D ).isDown ) RightMovement += 1;
        if ( UserInputHandle->GetKeyState( GLFW_KEY_A ).isDown ) RightMovement -= 1;

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
DEFINE_CONCEPT( CameraController )
DEFINE_SIMPLE_IMGUI_TYPE( CameraController, m_ViewControlSensitivity )

CameraController::~CameraController( )
{
    Engine::GetEngine( )->GetUserInputHandle( )->LockCursor( false );
}


class PhysicsScene
{
    void InitializeScene( );

public:
    PhysicsScene( physx::PxPhysics* Physics )
        : m_Physics( Physics )
    {
        physx::PxSceneDesc sceneDesc( Physics->getTolerancesScale( ) );
        sceneDesc.gravity       = physx::PxVec3( 0.0f, -9.81f, 0.0f );
        m_Dispatcher            = physx::PxDefaultCpuDispatcherCreate( 2 );
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

        m_ToleranceScale.length = 100;   // typical length of an object
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
        m_Foundation->release( );
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

class GroupConvexSerializer
{
public:
    enum class HitBoxType : uint8_t {
        eConvex   = 0,
        eTriangle = 1
    };

    GroupConvexSerializer( std::string Name, PhysicsEngine* PhyEngine )
        : m_GroupName( std::move( Name ) )
        , m_PhyEngine( PhyEngine )
    { }

    std::filesystem::path
    GetFilePath( )
    {
        return PATH_PREFIX / ( m_GroupName + "_convex_cache.bin" );
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

    template <typename Span>
        requires requires( Span Buffer ) {
            Buffer.size( );
            Buffer.data( );
        }
    void
    AppendCache( Span& VertexBuffer, uint16_t ConvexVertexLimit = 100 )
    {
        physx::PxConvexMeshDesc convexDesc;
        convexDesc.points.count  = VertexBuffer.size( );
        convexDesc.points.stride = sizeof( VertexBuffer[ 0 ] );
        convexDesc.points.data   = VertexBuffer.data( );
        convexDesc.flags         = physx::PxConvexFlag::eCOMPUTE_CONVEX
            //    | physx::PxConvexFlag::eCHECK_ZERO_AREA_TRIANGLES
            | physx::PxConvexFlag::eSHIFT_VERTICES;   // Need to verify
        convexDesc.vertexLimit = ConvexVertexLimit;

        physx::PxDefaultMemoryOutputStream     CookingBuffer;
        physx::PxConvexMeshCookingResult::Enum result;
        if ( !m_PhyEngine->GetCooking( ).cookConvexMesh( convexDesc, CookingBuffer, &result ) )
            throw std::runtime_error( "Failed to cook convex mesh" );

        const auto OldSize = m_CacheBuffer.size( );
        m_CacheBuffer.resize( OldSize + sizeof( size_t ) + sizeof( HitBoxType ) + CookingBuffer.getSize( ) );

        char* ChunkStart = m_CacheBuffer.data( ) + OldSize;

        // Buffer size
        size_t BufferSize = CookingBuffer.getSize( );
        memcpy( ChunkStart, &BufferSize, sizeof( size_t ) );
        // Buffer type
        *reinterpret_cast<HitBoxType*>( ChunkStart + sizeof( size_t ) ) = HitBoxType::eConvex;
        // Buffer data
        memcpy( ChunkStart + sizeof( size_t ) + sizeof( HitBoxType ), CookingBuffer.getData( ), CookingBuffer.getSize( ) );

        // Skip list save
        m_CacheBufferChunk.emplace_back( OldSize + sizeof( size_t ) + sizeof( HitBoxType ), CookingBuffer.getSize( ) );

        spdlog::info( "Cooked buffer size: {}", CookingBuffer.getSize( ) );
    }

    physx::PxRigidStatic*
    CreateRigidBodyFromCacheGroup( const physx::PxMaterial& material )
    {
        auto* hfActor = ( *m_PhyEngine )->createRigidStatic( physx::PxTransform( physx::PxVec3( 0 ) ) );
        hfActor->setName( m_GroupName.c_str( ) );

        for ( auto& Chunk : m_CacheBufferChunk )
        {
            physx::PxDefaultMemoryInputData input( (uint8_t*) m_CacheBuffer.data( ) + Chunk.first, Chunk.second );
            switch ( *reinterpret_cast<HitBoxType*>( m_CacheBuffer.data( ) + Chunk.first - sizeof( HitBoxType ) ) )
            {
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
            SerializerModel TestModel;

            TestModel.SetFilePath( "Assets/Model/low_poly_room.glb" );
            auto Mesh = PerspectiveCanvas->AddConcept<ConceptMesh>( );
            Mesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultPhongShader" ) );
            Mesh->SetShaderUniform( "lightPos", glm::vec3( 1.2f, 1.0f, 2.0f ) );
            Mesh->SetShaderUniform( "viewPos", m_MainCamera->GetCameraPosition( ) );
            Mesh->SetShaderUniform( "lightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );

            std::vector<SubMeshSpan> ModelSubMeshSpan;
            TestModel.ToMesh( Mesh.get( ), &ModelSubMeshSpan );

            TestModel.SetFilePath( "Assets/Model/red_cube.glb" );
            auto LightMesh = PerspectiveCanvas->AddConcept<ConceptMesh>( );
            LightMesh->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultMeshShader" ) );
            TestModel.ToMesh( LightMesh.get( ) );

            AddConcept<LightRotate>( Mesh, LightMesh );
            AddConcept<CameraController>( m_MainCamera );

            {
                m_PhyEngine = std::make_shared<PhysicsEngine>( );

                physx::PxMaterial*    Material    = ( *m_PhyEngine )->createMaterial( 0.5f, 0.5f, 0.6f );
                physx::PxRigidStatic* groundPlane = PxCreatePlane( *m_PhyEngine, physx::PxPlane( 0, 1, 0, 50 ), *Material );
                m_PhyEngine->GetScene( )->addActor( *groundPlane );

                {
                    GroupConvexSerializer GCS( "Room", m_PhyEngine.get( ) );
                    if ( GCS.TryLoad( ) )
                    {
                        m_PhyEngine->GetScene( )->addActor( *GCS.CreateRigidBodyFromCacheGroup( *Material ) );
                    } else
                    {
                        spdlog::info( "Number of ModelSubMeshSpan: {}", ModelSubMeshSpan.size( ) );

                        for ( auto& SubMeshSpan : ModelSubMeshSpan )
                            GCS.AppendCache( SubMeshSpan.VertexRange );

                        GCS.WriteCache( );
                        m_PhyEngine->GetScene( )->addActor( *GCS.CreateRigidBodyFromCacheGroup( *Material ) );
                    }
                }

                //                float              halfExtent = .5f;
                //                physx::PxShape*    shape      = ( *m_PhyEngine )->createShape( physx::PxBoxGeometry( halfExtent, halfExtent, halfExtent ), *Material );
                //                physx::PxU32       size       = 30;
                //                physx::PxTransform t( physx::PxVec3( 0 ) );
                //                for ( physx::PxU32 i = 0; i < size; i++ )
                //                {
                //                    for ( physx::PxU32 j = 0; j < size - i; j++ )
                //                    {
                //                        physx::PxTransform     localTm( physx::PxVec3( physx::PxReal( j * 2 ) - physx::PxReal( size - i ), physx::PxReal( i * 2 + 1 ), 0 ) * halfExtent );
                //                        physx::PxRigidDynamic* body = ( *m_PhyEngine )->createRigidDynamic( t.transform( localTm ) );
                //                        body->attachShape( *shape );
                //                        physx::PxRigidBodyExt::updateMassAndInertia( *body, 10.0f );
                //
                //                        body->setAngularVelocity( { (float) i, (float) j, 0 }, true );
                //
                //                        m_PhyEngine->GetScene( )->addActor( *body );
                //                    }
                //                }
                //
                //                shape->release( );
            }
        }
    }

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    m_PhyEngine->GetScene( )->simulate( 1 / 60.f );
    m_PhyEngine->GetScene( )->fetchResults( true );

    const auto SleepTime = 1000 / 60 - Engine::GetEngine( )->GetDeltaSecond( ) * 1000;
    if ( SleepTime > 0 )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( int( SleepTime ) ) );
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