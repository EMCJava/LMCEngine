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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

DEFINE_CONCEPT_DS( TileSpriteSet, ConceptRenderable )

void
TileSpriteSet::Render( )
{
    for ( const auto& Tile : m_TileList )
    {
        REQUIRED_IF( Tile.TextureCache != nullptr )
        {

            auto& Sp       = Tile.TextureCache;
            auto* SpShader = Sp->GetShader( );

            if ( SpShader != nullptr )
            {
                SpShader->Bind( );
                if ( m_ActiveCamera != nullptr )
                {
                    SpShader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
                }

                SpShader->SetMat4( "modelMatrix", m_TileMapOffsetMatrix * Tile.ModelMatrixCache );

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
    REQUIRED_IF( m_Sprites.contains( Tile.Degree ), spdlog::critical( "Missing {}", Tile.Degree ) )
    {
        Tile.TextureCache = m_Sprites[ Tile.Degree ].get( );

        Tile.AccumulatedDegree = Tile.Degree;
        if ( !m_TileList.empty( ) )
        {
            auto& LastTile = m_TileList.back( );

            /*
             *
             * Rotation stats
             *
             * */
            Tile.AccumulatedDegree += LastTile.AccumulatedDegree == 180 ? 0 : LastTile.AccumulatedDegree;


            /*
             *
             * Transformation matrix
             *
             * */
            Orientation TmpOrientation;
            TmpOrientation.SetCoordinate( TileDistance, 0 );

            TmpOrientation.SetRotationCenter( 512 / 2, 512 / 2 );
            TmpOrientation.SetRotation( 0, 0, glm::radians( 180 - FloatTy( LastTile.Degree ) ) );

            Tile.ModelMatrixCache = LastTile.ModelMatrixCache * TmpOrientation.GetRotationMatrix( ) * TmpOrientation.GetTranslationMatrix( );
        } else
        {
            Orientation TmpOrientation;
            TmpOrientation.SetOrigin( m_SpritesOrigin.X, m_SpritesOrigin.Y, m_SpritesOrigin.Z );

            Tile.ModelMatrixCache = TmpOrientation.GetTranslationMatrix( );
        }

        m_TileList.push_back( Tile );
    }
}

void
TileSpriteSet::SetSpritesOrigin( OrientationCoordinate::Coordinate Origin )
{
    m_SpritesOrigin = Origin;
}

uint32_t
TileSpriteSet::GetCurrentDegree( )
{
    REQUIRED_IF( !TileReachedEnd( ) )
    {
        return m_TileList[ m_TileListPointer ].Degree;
    }
}

void
TileSpriteSet::Advance( )
{
    REQUIRED_IF( !TileReachedEnd( ) )
    {
        m_TileListPointer++;

        UpdateTileMapOffset( );
    }
}

bool
TileSpriteSet::TileReachedEnd( )
{
    return m_TileListPointer >= m_TileList.size( );
}

FloatTy
TileSpriteSet::GetNextStartTime( )
{
    if ( m_TileListPointer >= m_TileList.size( ) - 1 )
    {
        return std::numeric_limits<FloatTy>::max( );
    }

    return m_TileList[ m_TileListPointer + 1 ].Time;
}

void
TileSpriteSet::SetTileMapOffset( const glm::mat4& OffsetMatrix )
{
    m_TileMapOffsetMatrix = OffsetMatrix;
}

FloatTy
TileSpriteSet::UpdateTileMapOffset( )
{
    const auto DecomposedDistance = sqrt( TileDistance * TileDistance / 2.0f );

    const auto RotationQuat      = glm::quat_cast( m_TileList[ m_TileListPointer ].ModelMatrixCache );
    const auto RollRotationAngle = glm::roll( RotationQuat );

    const auto      RotationAxis  = glm::vec2( DecomposedDistance, DecomposedDistance );
    const glm::vec2 RotatedOffset = glm::rotate( RotationAxis, RollRotationAngle );


    const auto Transform  = glm::vec3( m_TileList[ m_TileListPointer ].ModelMatrixCache[ 3 ] ) + glm::vec3( RotatedOffset, 0 );
    m_TileMapOffsetMatrix = glm::translate( glm::mat4( 1 ), -Transform );

    return RollRotationAngle;
}
