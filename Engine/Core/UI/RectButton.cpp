//
// Created by samsa on 8/31/2023.
//

#include "RectButton.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Concept/PureConceptAABBBox.hpp>

DEFINE_CONCEPT_DS( RectButton )

RectButton::RectButton( int Width, int Height )
{
    m_SpriteSquare = AddConcept<SpriteSquare>( Width, Height );

    m_SpriteSquare->SetRotationCenter( Width / 2, Height / 2 );
    m_SpriteSquare->SetOrigin( Width / 2, Height / 2 );

    m_SpriteSquare->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorShader" ) );
    m_SpriteSquare->SetAbsolutePosition( );
    m_SpriteSquare->SetupSprite( );

    m_HitBox = AddConcept<PureConceptAABBBox>( 0, 0, Width, Height );

    // To render sub-concepts
    SetSearchThrough( );
}

const OrientationCoordinate::Coordinate&
RectButton::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_HitBox->SetCoordinate( X, Y );
    return m_SpriteSquare->SetCoordinate( X, Y, Z );
}

void
RectButton::SetActiveCamera( class PureConceptCamera* ActiveCamera )
{
    m_SpriteSquare->SetActiveCamera( ActiveCamera );
}

void
RectButton::Apply( )
{
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
            spdlog::info( "[Button] -> Point is inside hitbox" );

            //            if ( m_Callback )
            //            {
            //                m_Callback( );
            //            }
        }
    }
}
