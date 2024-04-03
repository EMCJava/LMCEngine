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

    m_UICanvas = AddConcept<Canvas>( ).Get( );
    m_UICanvas->SetRuntimeName( "Login Canvas" );
    m_UICanvas->SetCanvasCamera( FixedCamera );

    auto LobbyText = m_UICanvas->AddConcept<Text>( "Lobby List" ).Get( );
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
        if ( Msg == "Created" )
        {
            m_InLobby = true;
            m_ServerConnection->Post( SanguisNet::Message::FromString( "", SanguisNet::MessageHeader::ID_LOBBY_LIST ) );
        }
        break;
    case SanguisNet::MessageHeader::ID_LOBBY_LIST:
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Names = Msg.ToString( ) ]( ) {
            std::ranges::for_each( m_LobbyMemberText, []( auto& Text ) { Text->Destroy( ); } );
            m_LobbyMemberText.clear( );
            for ( const auto& [ Index, Line ] :
                  Names
                      | std::ranges::views::split( '\n' )
                      | std::views::enumerate )
            {
                auto NewText = m_UICanvas->AddConcept<Text>( std::string( Line.begin( ), Line.end( ) ) ).Get( );
                NewText->SetupSprite( );
                NewText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
                NewText->SetColor( glm::vec3 { 1 } );
                NewText->SetCenterAt( glm::vec3 { 0, -50 * ( Index + 1 ), 0 } );
                m_LobbyMemberText.push_back( std::move( NewText ) );
            }
        } );
        break;
    }
}
