//
// Created by Lo Yu Sum on 14/8/2023.
//


#pragma once

#include "SpriteSquareTexture.hpp"

class SpriteSquareAnimatedTexture : public SpriteSquareTexture
{
    DECLARE_CONCEPT( SpriteSquareAnimatedTexture, SpriteSquareTexture )
public:
    using SpriteSquareTexture::SpriteSquareTexture;
    using FrameBoxListCoordinate = std::pair<FloatTy, FloatTy>;

    void
    Render( ) override;

    void
    SetupSprite( ) override;

    /*
     *
     * This will split the texture into grid and add to grid list,
     * assume starting from top left, move right then down
     *
     * */
    void
    SetTextureGrid( uint32_t HorCount, uint32_t VertCount );

    void
    AddGrid( FrameBoxListCoordinate TopLeft, FrameBoxListCoordinate BottomRight );

    void
    SetAnimationFrameIndex( uint32_t Index = 0 );

    void
    NextFrame( );

    size_t
    GetTotalFrame( ) const;

protected:
    uint32_t m_AnimationFrameIndex { 0 };
    uint32_t m_HorCount { 0 }, m_VertCount { 0 };

    std::vector<std::pair<FrameBoxListCoordinate, FrameBoxListCoordinate>> m_FrameBoxList { };

    ENABLE_IMGUI( SpriteSquareAnimatedTexture )
};
