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

    inline static constexpr auto TileDistance = 170 * 2;

public:
    struct TileMeta {
        uint32_t             Degree { };
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

protected:
    std::vector<TileMeta> m_TileList;

    /*
     *
     * Texture sprites for needed degrees
     *
     * */
    std::map<uint32_t, std::unique_ptr<SpriteSquareTexture>> m_Sprites;
};
