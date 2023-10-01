//
// Created by loyus on 8/1/2023.
//

#include "TileSpriteSet.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <sstream>

#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

DEFINE_CONCEPT_DS( TileSpriteSet )
DEFINE_SIMPLE_IMGUI_TYPE( TileSpriteSet, m_TileListPointer, m_SpritesOrigin, m_Sprites )

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

                SpShader->SetMat4( "modelMatrix", Tile.ModelMatrixCache );

                const auto* gl = Engine::GetEngine( )->GetGLContext( );
                gl->ActiveTexture( GL_TEXTURE0 );

                gl->BindTexture( GL_TEXTURE_2D, Sp->GetTextureID( ) );
                Sp->PureRender( );

                if ( Tile.ReverseDirection )
                {
                    gl->BindTexture( GL_TEXTURE_2D, m_ReverseSprites->GetTextureID( ) );
                    m_ReverseSprites->PureRender( );
                }
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

SpriteSquareTexture*
TileSpriteSet::RegisterReverseSprite( std::unique_ptr<SpriteSquareTexture>&& TSprite )
{
    auto* Backup     = TSprite.get( );
    m_ReverseSprites = std::move( TSprite );
    return Backup;
}

void
TileSpriteSet::AddTile( TileSpriteSet::TileMeta Tile )
{
    REQUIRED_IF( m_Sprites.contains( Tile.Degree ), ( PrintTileList( ), spdlog::critical( "Missing {}", Tile.Degree ) ) )
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

            TmpOrientation.SetRotationOrigin( glm::vec3 { 512 / 2, 512 / 2, 0 } );
            TmpOrientation.SetRotation( 0, 0, glm::radians( 180 - FloatTy( LastTile.Degree ) ) );

            Tile.ModelMatrixCache = LastTile.ModelMatrixCache * TmpOrientation.GetRotationMatrix( ) * TmpOrientation.GetTranslationMatrix( );
        } else
        {
            Orientation TmpOrientation;
            TmpOrientation.SetTranslationOrigin( m_SpritesOrigin );

            Tile.ModelMatrixCache = TmpOrientation.GetTranslationMatrix( );
        }

        m_TileList.push_back( Tile );
    }
}

void
TileSpriteSet::SetSpritesOrigin( glm::vec3 Origin )
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

    return 0;
}

void
TileSpriteSet::Advance( )
{
    REQUIRED_IF( !TileReachedEnd( ) )
    {
        m_TileListPointer++;
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

TileSpriteSet::TileMeta&
TileSpriteSet::GetCurrentTileMeta( )
{
    return m_TileList[ m_TileListPointer ];
}

void
TileSpriteSet::PrintTileList( )
{
    std::stringstream ss;

    for ( const auto& Tile : m_TileList )
    {
        ss << "{" << Tile.Degree << ',' << Tile.Time << ( Tile.ReverseDirection ? ",true" : "" ) << "},\n";
    }

    spdlog::info( "{}", ss.str( ) );
}

void
TileSpriteSet::Retreat( )
{
    REQUIRED_IF( m_TileListPointer != 0 )
    {
        m_TileListPointer--;

        // UpdateTileMapOffset( );
    }
}

void
TileSpriteSet::RemoveLastTile( )
{
    if ( !m_TileList.empty( ) )
    {
        m_TileList.pop_back( );

        if ( m_TileListPointer >= m_TileList.size( ) )
        {
            Retreat( );
        }
    }
}

size_t
TileSpriteSet::GetTileCount( )
{
    return m_TileList.size( );
}

size_t
TileSpriteSet::GetCurrentPosition( ) const
{
    return m_TileListPointer;
}

glm::vec3
TileSpriteSet::GetCurrentTileTransform( )
{
    const auto DecomposedDistance = sqrt( TileDistance * TileDistance / 2.0f );

    const auto      RotationAxis  = glm::vec2( DecomposedDistance, DecomposedDistance );
    const glm::vec2 RotatedOffset = glm::rotate( RotationAxis, GetCurrentRollRotation( ) );

    return glm::vec3( m_TileList[ m_TileListPointer ].ModelMatrixCache[ 3 ] ) + glm::vec3( RotatedOffset, 0 );
}

FloatTy
TileSpriteSet::GetCurrentRollRotation( ) const
{
    const auto RotationQuat = glm::quat_cast( m_TileList[ m_TileListPointer ].ModelMatrixCache );
    return glm::roll( RotationQuat );
}
