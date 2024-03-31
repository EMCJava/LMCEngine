//
// Created by samsa on 8/31/2023.
//

#include "RectButton.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/UI/Text/Text.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Concept/PureConceptAABBSquare.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( RectButton )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RectButton, PureConcept, m_DefaultColor, m_PressColor, m_TextAlignmentOffset, m_PressReactTimeLeft )

RectButton::RectButton( int Width, int Height )
    : m_UserDefinedWidth( Width )
    , m_UserDefinedHeight( Height )
{
    SetupButton( );
    SetSearchThrough( );
}

const glm::vec3&
RectButton::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z )
{
    const std::pair<FloatTy, FloatTy> PivotOffset = { m_Pivot.first * m_Width, m_Pivot.second * m_Height };
    m_HitBox->SetCoordinate( X - PivotOffset.first, Y - PivotOffset.second );
    m_ButtonText->SetCoordinate( glm::vec3 { X + m_TextAlignmentOffset.first - PivotOffset.first, Y + m_TextAlignmentOffset.second - PivotOffset.second, 0 } );
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
    m_ActivatedThisFrame = false;
    if ( !m_Enabled ) return;

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
        REQUIRED_IF( ActiveCamera != nullptr && ActiveCamera->CanCastVT<PureConceptOrthoCamera>( ) )
        {
            ( (PureConceptOrthoCamera*) ActiveCamera )->ScreenCoordToUICoord( HitPoint );
        }

        if ( m_HitBox->HitTest( HitPoint ) )
        {
            m_ActivatedThisFrame = true;
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

void
RectButton::SetupButton( )
{
    m_ButtonText.reset( );
    m_SpriteSquare.reset( );
    m_HitBox.reset( );

    RemoveConcepts<PureConcept>( );

    m_ButtonText = AddConcept<Text>( m_ButtonTextStr );
    m_ButtonText->SetupSprite( );
    m_ButtonText->SetFont( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Font>( "DefaultFont" ) );
    m_ButtonText->SetColor( m_TextColor );

    m_Width  = m_UserDefinedWidth;
    m_Height = m_UserDefinedHeight;

    const int WidthSpacing  = m_Width < 0 ? -m_Width : 0;
    const int HeightSpacing = m_Height < 0 ? -m_Height : 0;

    if ( m_Width <= 0 ) m_Width = m_ButtonText->GetTextPixelWidth( );
    if ( m_Height <= 0 ) m_Height = m_ButtonText->GetTextPixelHeight( );

    if ( WidthSpacing != 0 ) m_Width += WidthSpacing;
    if ( HeightSpacing != 0 ) m_Height += HeightSpacing;

    m_TextAlignmentOffset = { ( m_Width - (int) m_ButtonText->GetTextPixelWidth( ) ) / 2.F,
                              ( m_Height - (int) m_ButtonText->GetTextPixelHeight( ) ) / 2.F };

    m_SpriteSquare = AddConcept<SpriteSquare>( m_Width, m_Height );

    m_SpriteSquare->SetOrigin( m_Width * m_Pivot.first, m_Height * m_Pivot.second );

    m_SpriteSquare->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorShader" ) );
    m_SpriteSquare->SetupSprite( );

    m_HitBox = AddConcept<PureConceptAABBSquare>( 0, 0, m_Width, m_Height );

    m_SpriteSquare->MoveToFirstAsSubConcept( );

    // Set alignment by default
    SetCoordinate( );

    // To render sub-concepts
    SetSearchThrough( );
}

void
RectButton::SetText( const std::string& Text )
{
    m_ButtonTextStr = Text;
    SetupButton( );
}

void
RectButton::SetPivot( FloatTy X, FloatTy Y )
{
    m_Pivot = { X, Y };
    SetupButton( );
}

void
RectButton::SetEnabled( bool Enabled )
{
    PureConcept::SetEnabled( Enabled );

    ForEachSubConcept( [ Enabled ]( auto& SubConcept ) {
        SubConcept->SetEnabled( Enabled );
    } );
}

void
RectButton::SetDefaultColor( const glm::vec4& Color )
{
    m_DefaultColor = Color;
}

void
RectButton::SetTextColor( const glm::vec3& Color )
{
    m_TextColor = Color;
}
