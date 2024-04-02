//
// Created by EMCJava on 4/2/2024.
//

#include "LoginScene.hpp"

#include <Engine/Core/UI/RectInput.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS( LoginScene )

LoginScene::LoginScene( const std::function<void( const std::string&, const std::string& )>& LoginFnc )
    : m_LoginFnc( LoginFnc )
{
    SetSearchThrough( true );
    SetRuntimeName( "Login Screen" );

    auto FixedCamera = AddConcept<PureConceptOrthoCamera>( ).Get( );

    FixedCamera->SetScale( 1 / 1.5F );
    FixedCamera->UpdateCameraMatrix( );

    auto UICanvas = AddConcept<Canvas>( ).Get( );
    UICanvas->SetRuntimeName( "Login Canvas" );
    UICanvas->SetCanvasCamera( FixedCamera );

    m_NameInput = UICanvas->AddConcept<RectInput>( -25, 60 );
    m_NameInput->SetPressReactColor( glm::vec4 { 0.5, 0.5, 0.5, 1 } );
    m_NameInput->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
    m_NameInput->SetTextColor( glm::vec3 { 1, 1, 1 } );
    m_NameInput->SetText( "Player1" );
    m_NameInput->SetPivot( 0.5F, 0.5F );
    m_NameInput->SetCoordinate( 0, 70 );

    m_PasswordInput = UICanvas->AddConcept<RectInput>( -25, 60 );
    m_PasswordInput->SetPressReactColor( glm::vec4 { 0.5, 0.5, 0.5, 1 } );
    m_PasswordInput->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
    m_PasswordInput->SetTextColor( glm::vec3 { 1, 1, 1 } );
    m_PasswordInput->SetText( "1" );
    m_PasswordInput->SetPivot( 0.5F, 0.5F );
    m_PasswordInput->SetCoordinate( 0, 0 );

    auto LoginButton = UICanvas->AddConcept<RectButton>( -25, 60 ).Get( );
    LoginButton->SetPressReactColor( glm::vec4 { 0.5, 0.5, 0.5, 1 } );
    LoginButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
    LoginButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
    LoginButton->SetText( "Login" );
    LoginButton->SetPivot( 0.5F, 0.5F );
    LoginButton->SetCoordinate( 0, -70 );
    LoginButton->SetCallback( [ this ]( ) {
        m_LoginFnc( m_NameInput->GetText( ), m_PasswordInput->GetText( ) );
        Destroy( );
    } );
}
