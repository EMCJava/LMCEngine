//
// Created by samsa on 8/31/2023.
//

#include "RectButton.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/UI/Text/Text.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Concept/PureConceptAABBBox.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( RectButton )
DEFINE_SIMPLE_IMGUI_TYPE( RectButton, m_DefaultColor, m_PressColor, m_TextAlignmentOffset, m_PressReactTimeLeft )

RectButton::RectButton( int Width, int Height )
{
    m_ButtonText = AddConcept<Text>( "Button" );
    m_ButtonText->SetupSprite( );
    m_ButtonText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );

    spdlog::critical( "GetGlobalResourcePool: {}", fmt::ptr( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ).get( ) ) );

    const int WidthSpacing  = Width < 0 ? -Width : 0;
    const int HeightSpacing = Height < 0 ? -Height : 0;

    if ( Width <= 0 ) Width = m_ButtonText->GetTextPixelWidth( );
    if ( Height <= 0 ) Height = m_ButtonText->GetTextPixelHeight( );

    if ( WidthSpacing != 0 ) Width += WidthSpacing;
    if ( HeightSpacing != 0 ) Height += HeightSpacing;

    m_TextAlignmentOffset = { ( Width - (int) m_ButtonText->GetTextPixelWidth( ) ) / 2.F,
                              ( Height - (int) m_ButtonText->GetTextPixelHeight( ) ) / 2.F };

    m_SpriteSquare = AddConcept<SpriteSquare>( Width, Height );

    m_SpriteSquare->SetRotationCenter( Width / 2, Height / 2 );
    m_SpriteSquare->SetOrigin( Width / 2, Height / 2 );

    m_SpriteSquare->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorShader" ) );
    m_SpriteSquare->SetAbsolutePosition( );
    m_SpriteSquare->SetupSprite( );

    m_HitBox = AddConcept<PureConceptAABBBox>( 0, 0, Width, Height );

    m_SpriteSquare->MoveToFirstAsSubConcept( );

    // Set alignment by default
    SetCoordinate( );

    // To render sub-concepts
    SetSearchThrough( );
}

const OrientationCoordinate::Coordinate&
RectButton::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_HitBox->SetCoordinate( X, Y );
    m_ButtonText->SetCoordinate( OrientationCoordinate::Coordinate { X + m_TextAlignmentOffset.first, Y + m_TextAlignmentOffset.second } );
    return m_SpriteSquare->SetCoordinate( X, Y, Z );
}

void
RectButton::SetActiveCamera( class PureConceptCamera* ActiveCamera )
{
    m_SpriteSquare->SetActiveCamera( ActiveCamera );
    m_ButtonText->SetActiveCamera( ActiveCamera );
}

void
RectButton::Apply( )
{
    if ( m_PressReactTimeLeft > 0 )
    {
        m_PressReactTimeLeft -= Engine::GetEngine( )->GetDeltaSecond( );
        m_SpriteSquare->SetShaderUniform( "fragColor", m_PressColor );
    } else
    {
        m_SpriteSquare->SetShaderUniform( "fragColor", m_DefaultColor );
    }

    if ( Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( ).isPressed )
    {
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );

        const auto* ActiveCamera = m_SpriteSquare->GetActiveCamera( );
        REQUIRED_IF( m_SpriteSquare->GetActiveCamera( ) )
        {
            ActiveCamera->ScreenCoordToUICoord( HitPoint );
        }

        if ( m_HitBox->HitTest( HitPoint ) )
        {
            m_PressReactTimeLeft = 0.2;

            if ( m_Callback )
            {
                m_Callback( );
            }
        }
    }
}

void
RectButton::SetPressReactColor( const glm::vec4& Color )
{
    m_PressColor = Color;
}
