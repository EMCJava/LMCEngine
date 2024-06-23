//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"

#include "WandEditorScene.hpp"
#include "LoginScene.hpp"
#include "LobbyScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/UI/RectInput.hpp>
#include <Engine/Core/Graphic/Mesh/RenderableMesh.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Physics/PhysicsEngine.hpp>
#include <Engine/Core/Physics/PhysicsScene.hpp>
#include <Engine/Core/Physics/Collider/Serializer/ColliderSerializerGroupMesh.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Physics/Collider/ColliderMesh.hpp>
#include <Engine/Core/Physics/RigidBody/RigidMesh.hpp>
#include <Engine/Core/Physics/Controller/PhyControllerEntity.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <PxPhysicsAPI.h>

#include <filesystem>
#include <utility>

GameManager::~GameManager( )
{
    spdlog::trace( "{}::~{} -> {}", "GameManager", "GameManager", fmt::ptr( this ) );
    m_ServerConnection.reset( );
    m_IOContextHolder.reset( );
    m_IOContext->stop( );
    if ( m_IOThread && m_IOThread->joinable( ) )
        m_IOThread->join( );
}

DEFINE_CONCEPT_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, ConceptApplicable, TestInvokable )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    constexpr std::pair<int, int> WindowSize = { 1920, 1080 };
    Engine::GetEngine( )->SetLogicalMainWindowViewPortDimensions( WindowSize );

    m_IOContext = std::make_shared<asio::io_context>( );

    asio::ip::tcp::resolver resolver( *m_IOContext );
    auto                    endpoints = resolver.resolve( "localhost", "8800" );
    m_ServerConnection                = std::make_shared<SanguisNet::ClientGroupParticipant>( *m_IOContext, endpoints );
    m_ServerConnection->SetConnectCallback( [ this ]( ) {
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this ]( ) {
            AddConcept<LoginScene>( [ this ]( const std::string& Name, const std::string& Password ) {
                std::string login = Name + '\0' + Password;
                m_ServerConnection->SetPacketCallback( [ this, Name ]( SanguisNet::Message& Msg ) {
                    if ( Msg.header.id == SanguisNet::MessageHeader::ID_RESULT && Msg == "Login Success" )
                        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Name ]( ) { AddConcept<LobbyScene>( m_ServerConnection, Name ); } );
                } );
                m_ServerConnection->Post( SanguisNet::Message::FromString( login, SanguisNet::MessageHeader::ID_LOGIN ) );
            } );
        } );
    } );

    m_IOContextHolder = std::make_shared<asio::io_context::work>( *m_IOContext );
    m_IOThread        = std::make_unique<std::thread>( [ this ]( ) {
        spdlog::info( "Net IO thread started" );
        m_IOContext->run( );
        spdlog::info( "Net IO thread stopped" );
    } );

    spdlog::info( "GameManager concept constructor returned" );
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
            m_WandEditorScene = AddConcept<WandEditorScene>( ).Get( );
        }
    }
}