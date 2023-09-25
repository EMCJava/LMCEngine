//
// Created by loyus on 23/7/2023.
//

#pragma once

#include "SpriteSquare.hpp"

class SpriteSquareTexture : public SpriteSquare
{
    DECLARE_CONCEPT( SpriteSquareTexture, SpriteSquare )

public:
    using SpriteSquare::SpriteSquare;

    /*
     *
     * This will set size to the image and SetupSprite will be called
     *
     * */
    SpriteSquareTexture( const std::shared_ptr<Shader>& Sha, const std::shared_ptr<class PureConceptImage>& Img );

    void
    SetTexturePath( const std::string& TexturePath );

    void
    SetImage( std::shared_ptr<class PureConceptImage> Image );

    void Render( ) override;

    void
    SetupSprite( ) override;

    void
    LoadTexture( );

    void
    BindTexture( );

    uint32_t
    GetTextureID( ) const noexcept;

protected:
    uint32_t    m_TextureID;
    std::string m_TexturePath;

    std::shared_ptr<class PureConceptImage> m_TextureImage;

    ENABLE_IMGUI( SpriteSquareTexture )
};
