//
// Created by loyus on 7/12/2023.
//

#pragma once

#include "Sprite.hpp"

class SpriteSquare : public Sprite
{
    DECLARE_CONCEPT( SpriteSquare, Sprite )

public:
    SpriteSquare( int Width, int Height );

    void
    Render( ) override;


    /*
     *
     * Render but don't change anything in the shader
     *
     * */
    void
    PureRender( );

    void
    SetupSprite( ) override;

protected:
    int m_Width = 0, m_Height = 0;

    ENABLE_IMGUI( SpriteSquare )
};
