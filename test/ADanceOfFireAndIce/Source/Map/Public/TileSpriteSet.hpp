//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

#include <memory>
#include <map>

class TileSpriteSet : public ConceptRenderable
{
    DECLARE_CONCEPT( TileSpriteSet, ConceptRenderable )

public:
    inline static constexpr auto TileDistance = 170 * 2;

    struct TileMeta {
        uint32_t Degree { };
        FloatTy  Time { };

        bool    ReverseDirection { false };
        FloatTy BPMChange { 0.0f };

        SpriteSquareTexture* TextureCache { };

        uint32_t  AccumulatedDegree { };
        glm::mat4 ModelMatrixCache { 1 };
    };

    void
    Render( ) override;

    SpriteSquareTexture*
    RegisterSprite( uint32_t Degree, std::unique_ptr<SpriteSquareTexture>&& TSprite );

    void
    AddTile( TileMeta Tile );

    void RemoveLastTile( );

    void
    SetSpritesOrigin( OrientationCoordinate::Coordinate Origin );

    /*
     *
     * Following functions are for tile set stage
     *
     * */

    bool
    TileReachedEnd( );

    /*
     *
     * Return the current degree of the pointer
     *
     * */
    uint32_t
    GetCurrentDegree( );

    size_t
    GetCurrentPosition( ) const;

    /*
     *
     * Return the start time of next tile
     *
     * */
    FloatTy
    GetNextStartTime( );

    TileMeta&
    GetCurrentTileMeta( );

    size_t
    GetTileCount( );

    /*
     *
     * Move pointer to the next tile
     *
     * */
    void
    Advance( );

    /*
     *
     * Move pointer to the previous tile
     *
     * */
    void
    Retreat( );

    /*
     *
     * Move map to current coordinate
     * Return current tile rotation
     *
     * */
    FloatTy
    UpdateTileMapOffset( );

    void
    SetTileMapOffset( const glm::mat4& OffsetMatrix );

    /*
     *
     * For debug purpose
     *
     * */
    void
    PrintTileList( );

protected:
    size_t                m_TileListPointer { 0 };
    std::vector<TileMeta> m_TileList;

    OrientationCoordinate::Coordinate m_SpritesOrigin { };

    glm::mat4 m_TileMapOffsetMatrix { 1 };

    /*
     *
     * Texture sprites for needed degrees
     *
     * */
    std::map<uint32_t, std::unique_ptr<SpriteSquareTexture>> m_Sprites;
};
