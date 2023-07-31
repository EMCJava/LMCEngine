//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <stdint.h>

/*
 *
 * Inspired from https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
 *
 * */
class HotReloadFrameBuffer
{

public:
    HotReloadFrameBuffer( ) = default;
    ~HotReloadFrameBuffer( );

    void
    CreateFrameBuffer( float Width, float Height );

    void
    BindFrameBuffer( ) const;

    static void
    UnBindFrameBuffer( );

    void
    RescaleFrameBuffer( float Width, float Height );

    uint32_t
    GetTextureID( ) const;

private:
    uint32_t m_FBO { }, m_RBO { };
    uint32_t m_TextureID { };
};
