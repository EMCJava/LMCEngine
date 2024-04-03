//
// Created by EMCJava on 4/2/2024.
//

#include "LobbyScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/UI/Text/Text.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>


DEFINE_CONCEPT_DS( LobbyScene )

LobbyScene::LobbyScene( const std::shared_ptr<SanguisNet::ClientGroupParticipant>& Connection )
    : m_ServerConnection( Connection )
{
    SetSearchThrough( true );
    SetRuntimeName( "Lobby Screen" );

    auto FixedCamera = AddConcept<PureConceptOrthoCamera>( ).Get( );

    FixedCamera->SetScale( 1 / 1.5F );
    FixedCamera->UpdateCameraMatrix( );

    auto UICanvas = AddConcept<Canvas>( ).Get( );
    UICanvas->SetRuntimeName( "Login Canvas" );
    UICanvas->SetCanvasCamera( FixedCamera );

    auto LobbyText = UICanvas->AddConcept<Text>( "Lobby List" ).Get( );
    LobbyText->SetupSprite( );
    LobbyText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
    LobbyText->SetColor( glm::vec3 { 1 } );
    LobbyText->SetCenterAt( glm::vec3 { 0, 0, 0 } );

    m_ServerConnection->SetPacketCallback( [ this ]( auto&& Msg ) { ServerMessageCallback( Msg ); } );
    m_ServerConnection->Post( SanguisNet::Message::FromString( "create", SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
}

void
LobbyScene::ServerMessageCallback( const SanguisNet::Message& Msg )
{
    switch ( Msg.header.id )
    {
    case SanguisNet::MessageHeader::ID_RESULT:
        m_InLobby = Msg == "Created";
        break;
    }
}
