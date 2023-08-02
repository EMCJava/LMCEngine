//
// Created by loyus on 8/1/2023.
//

#include "TileSpriteSet.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( TileSpriteSet, ConceptRenderable )

void
TileSpriteSet::Render( )
{
    glm::mat4 TransformationMatrix { 1 };

    Orientation TmpOrientation;
    TmpOrientation.SetRotationCenter( 512 / 2, 512 / 2 );
    TmpOrientation.SetCoordinate( -170, -200 );
    TransformationMatrix = TmpOrientation.GetTranslationMatrix( ) * TmpOrientation.GetRotationMatrix( );

    // Travel for next tile
    TmpOrientation.SetCoordinate( TileDistance, 0 );

    auto CameraSize = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );
    CameraSize.first *= 5;
    CameraSize.second *= 5;

    for ( const auto& Tile : m_TileList )
    {
        REQUIRED_IF( Tile.TextureCache != nullptr )
        {

            auto& Sp       = Tile.TextureCache;
            auto* SpShader = Sp->GetShader( );

            if ( SpShader != nullptr )
            {
                SpShader->Bind( );
//                if ( m_ActiveCamera != nullptr )
//                {
//                    SpShader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
//                }

                SpShader->SetMat4( "projectionMatrix", glm::ortho<FloatTy>( -CameraSize.first / 2, CameraSize.first / 2, -CameraSize.second / 2, CameraSize.second / 2, -1, 1 ) );

                SpShader->SetMat4( "modelMatrix", TransformationMatrix );

                TmpOrientation.SetRotation( 0, 0, glm::radians( 180 - FloatTy( Tile.Degree ) ) );
                TransformationMatrix = TransformationMatrix * TmpOrientation.GetRotationMatrix( ) * TmpOrientation.GetTranslationMatrix( );

                const auto* gl = Engine::GetEngine( )->GetGLContext( );
                gl->ActiveTexture( GL_TEXTURE0 );
                gl->BindTexture( GL_TEXTURE_2D, Sp->GetTextureID( ) );

                Sp->PureRender( );
            }
        }
    }
}

SpriteSquareTexture*
TileSpriteSet::RegisterSprite( uint32_t Degree, std::unique_ptr<SpriteSquareTexture>&& TSprite )
{
    if ( m_Sprites.contains( Degree ) )
    {
        spdlog::warn( "Overriding existing sprite for degree {}", Degree );
    }

    auto* Backup        = TSprite.get( );
    m_Sprites[ Degree ] = std::move( TSprite );
    return Backup;
}

void
TileSpriteSet::AddTile( TileSpriteSet::TileMeta Tile )
{
    REQUIRED_IF( m_Sprites.contains( Tile.Degree ) )
    {
        Tile.TextureCache = m_Sprites[ Tile.Degree ].get( );
    }

    Tile.AccumulatedDegree = Tile.Degree;
    if ( !m_TileList.empty( ) )
    {
        Tile.AccumulatedDegree += m_TileList.back( ).AccumulatedDegree;
    }

    m_TileList.push_back( Tile );
}
