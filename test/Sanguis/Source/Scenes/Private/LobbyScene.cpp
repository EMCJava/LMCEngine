//
// Created by EMCJava on 4/2/2024.
//

#include "LobbyScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/UI/Text/Text.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <utility>


DEFINE_CONCEPT_DS( LobbyScene )

LobbyScene::LobbyScene( const std::shared_ptr<SanguisNet::ClientGroupParticipant>& Connection, std::string UserName )
    : m_UserName( std::move( UserName ) )
    , m_ServerConnection( Connection )
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

    m_ReadyButton = m_UICanvas->AddConcept<RectButton>( -25, 60 ).Get( );
    m_ReadyButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
    m_ReadyButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
    m_ReadyButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
    m_ReadyButton->SetText( "Ready" );
    m_ReadyButton->SetPivot( 0.5F, 0.5F );
    m_ReadyButton->SetCoordinate( 0, 100 );
    m_ReadyButton->SetActiveCamera( FixedCamera.get( ) );
    m_ReadyButton->SetCallback( [ this ]( ) {
        if ( m_IsReady )
        {
            m_ServerConnection->Post( SanguisNet::Message::FromString( "cancel_ready", SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
        } else
        {
            m_ServerConnection->Post( SanguisNet::Message::FromString( "ready", SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
        }
    } );

    m_ServerConnection->SetPacketCallback( [ this ]( auto&& Msg ) { ServerMessageCallback( Msg ); } );
    m_ServerConnection->Post( SanguisNet::Message::FromString( "create", SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
}

void
LobbyScene::ServerMessageCallback( const SanguisNet::Message& Msg )
{
    const auto MshStrView = Msg.ToString( );
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
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Names = std::string( MshStrView.begin( ), MshStrView.end( ) ) ]( ) {
            std::ranges::for_each( m_LobbyMemberText, []( auto& Text ) { Text->Destroy( ); } );
            m_LobbyMemberText.clear( );

            size_t Index = 0;
            for ( const auto& Line :
                  Names | std::ranges::views::split( '\n' ) )
            {
                auto NewText = m_UICanvas->AddConcept<Text>( std::string( Line.begin( ), Line.end( ) ) ).Get( );
                NewText->SetupSprite( );
                NewText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
                NewText->SetColor( glm::vec3 { 1, 0, 0 } );
                NewText->SetCenterAt( glm::vec3 { 0, -50 * ++Index, 0 } );
                m_LobbyMemberText.push_back( std::move( NewText ) );
            }
        } );
        break;
    case SanguisNet::MessageHeader::ID_INFO:
        if ( Msg.EndWith( " Entered" ) )
        {
            Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Name = std::string( MshStrView.data( ), MshStrView.find( " Entered" ) ) ]( ) {
                auto NewText = m_UICanvas->AddConcept<Text>( std::string( Name.begin( ), Name.end( ) ) ).Get( );
                NewText->SetupSprite( );
                NewText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
                NewText->SetColor( glm::vec3 { 1 } );
                NewText->SetCenterAt( glm::vec3 { 0, -50 * ( (int) m_LobbyMemberText.size( ) + 1 ), 0 } );
                m_LobbyMemberText.push_back( std::move( NewText ) );
            } );
        } else if ( Msg.EndWith( " Left" ) )
        {
            Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Name = std::string( MshStrView.data( ), MshStrView.find( " Left" ) ) ]( ) {
                for ( int i = 0; i < m_LobbyMemberText.size( ); ++i )
                {
                    if ( m_LobbyMemberText[ i ]->GetText( ) == Name )
                    {
                        m_LobbyMemberText[ i ]->Destroy( );
                        m_LobbyMemberText.erase( m_LobbyMemberText.begin( ) + i-- );
                        continue;
                    }
                    m_LobbyMemberText[ i ]->SetCenterAt( glm::vec3 { 0, -50 * ( i + 1 ), 0 } );
                }
            } );
        } else if ( Msg.EndWith( " Ready" ) )
        {
            Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Name = std::string( MshStrView.data( ), MshStrView.find( " Ready" ) ) ]( ) {
                auto PlayerIt = std::ranges::find_if( m_LobbyMemberText, [ &Name ]( std::shared_ptr<Text>& Text ) {
                    return Text->GetText( ) == Name;
                } );
                if ( PlayerIt != m_LobbyMemberText.end( ) )
                {
                    ( *PlayerIt )->SetColor( glm::vec3 { 0, 1, 0 } );
                }
                if ( m_UserName == Name )
                {
                    m_ReadyButton->SetDefaultColor( glm::vec4 { 255 / 255.0, 78 / 255.0, 30 / 255.0, 1 } );
                    m_ReadyButton->SetText( "Cancel" );
                    m_IsReady = true;
                }
            } );
        } else if ( Msg.EndWith( " CancelReady" ) )
        {
            Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Name = std::string( MshStrView.data( ), MshStrView.find( " CancelReady" ) ) ]( ) {
                auto PlayerIt = std::ranges::find_if( m_LobbyMemberText, [ &Name ]( std::shared_ptr<Text>& Text ) {
                    return Text->GetText( ) == Name;
                } );
                if ( PlayerIt != m_LobbyMemberText.end( ) )
                {
                    ( *PlayerIt )->SetColor( glm::vec3 { 1, 0, 0 } );
                }
                if ( m_UserName == Name )
                {
                    m_ReadyButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
                    m_ReadyButton->SetText( "Ready" );
                    m_IsReady = false;
                }
            } );
        }
        break;
    }
}
