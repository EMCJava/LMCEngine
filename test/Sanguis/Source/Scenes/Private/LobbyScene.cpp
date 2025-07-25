//
// Created by EMCJava on 4/2/2024.
//

#include "LobbyScene.hpp"
#include "GameScene.hpp"

#include "ClientGroupParticipant.hpp"

#include <Engine/Core/UI/Text/Text.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>

#include <utility>
#include <ranges>

constexpr int NameSpace = 70;

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

    auto PlayerText = m_UICanvas->AddConcept<Text>( "Player List" ).Get( );
    PlayerText->SetupSprite( );
    PlayerText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
    PlayerText->SetColor( glm::vec3 { 1 } );
    PlayerText->SetCenterAt( glm::vec3 { -500, 0, 0 } );

    auto FriendText = m_UICanvas->AddConcept<Text>( "Friend List" ).Get( );
    FriendText->SetupSprite( );
    FriendText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
    FriendText->SetColor( glm::vec3 { 1 } );
    FriendText->SetCenterAt( glm::vec3 { 500, 0, 0 } );

    auto LobbyText = m_UICanvas->AddConcept<Text>( "Lobby" ).Get( );
    LobbyText->SetupSprite( );
    LobbyText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
    LobbyText->SetColor( glm::vec3 { 1 } );
    LobbyText->SetScale( 3 );
    LobbyText->SetCenterAt( glm::vec3 { 0, 600, 0 } );

    m_ReadyButton = m_UICanvas->AddConcept<RectButton>( -25, 60 ).Get( );
    m_ReadyButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
    m_ReadyButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
    m_ReadyButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
    m_ReadyButton->SetText( "Ready" );
    m_ReadyButton->SetPivot( 0.5F, 0.5F );
    m_ReadyButton->SetCoordinate( -500, 100 );
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
    m_ServerConnection->Post( SanguisNet::Message::FromString( "0", SanguisNet::MessageHeader::ID_FRIEND_LIST ) );
}

void
LobbyScene::ServerMessageCallback( const SanguisNet::Message& Msg )
{
    const auto MshStrView = Msg.ToString( );
    switch ( Msg.header.id )
    {
    case SanguisNet::MessageHeader::ID_RESULT:
        if ( Msg == "Created" ) m_InLobby = true;
        break;
    case SanguisNet::MessageHeader::ID_FRIEND_LIST:
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Names = std::string( MshStrView.begin( ), MshStrView.end( ) ) ]( ) {
            std::ranges::for_each( m_FriendListText, []( auto& Text ) { Text->Destroy( ); } );
            m_FriendListText.clear( );
            std::ranges::for_each( m_FriendListJoinButton, []( auto& Text ) { Text->Destroy( ); } );
            m_FriendListJoinButton.clear( );

            int Index = 0;
            for ( const auto& Line :
                  Names | std::ranges::views::split( '\n' ) )
            {
                auto NewText = m_UICanvas->AddConcept<Text>( std::string( Line.begin( ), Line.end( ) ) ).Get( );
                NewText->SetupSprite( );
                NewText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
                NewText->SetColor( glm::vec3 { 1 } );
                NewText->SetCenterAt( glm::vec3 { 500, -NameSpace * ++Index, 0 } );
                m_FriendListText.push_back( std::move( NewText ) );

                auto JoinButton = m_UICanvas->AddConcept<RectButton>( -25, 60 ).Get( );
                JoinButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
                JoinButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
                JoinButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
                JoinButton->SetText( "Join" );
                JoinButton->SetPivot( 0.5F, 0.5F );
                JoinButton->SetCoordinate( 800, -NameSpace * Index );
                JoinButton->SetActiveCamera( GetConcept<PureConceptOrthoCamera>( ).get( ) );
                JoinButton->SetCallback( [ this, Name = std::string { Line.begin( ), Line.end( ) } ]( ) {
                    m_ServerConnection->Post( SanguisNet::Message::FromString( "join" + Name, SanguisNet::MessageHeader::ID_LOBBY_CONTROL ) );
                } );
                m_FriendListJoinButton.push_back( std::move( JoinButton ) );
            }
        } );
        break;
    case SanguisNet::MessageHeader::ID_LOBBY_LIST:
        Engine::GetEngine( )->PushPostConceptUpdateCall( [ this, Names = std::string( MshStrView.begin( ), MshStrView.end( ) ) ]( ) {
            std::ranges::for_each( m_LobbyMemberText, []( auto& Text ) { Text->Destroy( ); } );
            m_LobbyMemberText.clear( );

            int Index = 0;
            for ( const auto& Line :
                  Names | std::ranges::views::split( '\n' ) )
            {
                auto NewText = m_UICanvas->AddConcept<Text>( std::string( Line.begin( ), Line.end( ) - 1 ) ).Get( );
                NewText->SetupSprite( );
                NewText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
                if ( Line.back( ) == 't' )
                    NewText->SetColor( glm::vec3 { 0, 1, 0 } );
                else
                    NewText->SetColor( glm::vec3 { 1, 0, 0 } );
                NewText->SetCenterAt( glm::vec3 { -500, -NameSpace * ++Index, 0 } );
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
                NewText->SetColor( glm::vec3 { 1, 0, 0 } );
                NewText->SetCenterAt( glm::vec3 { -500, -NameSpace * ( (int) m_LobbyMemberText.size( ) + 1 ), 0 } );
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
                    m_LobbyMemberText[ i ]->SetCenterAt( glm::vec3 { -500, -NameSpace * ( i + 1 ), 0 } );
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
        } else if ( Msg == "AllReady" )
        {
            Engine::GetEngine( )->PushPostConceptUpdateCall( [ this ]( ) {
                GetOwner( )->AddConcept<GameScene>( std::move( m_ServerConnection ), m_UserName );
                Destroy( );
            } );
        }
        break;
    }
}
