//
// Created by samsa on 12/21/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include <string>
#include <memory>

class Texture
{
public:
    Texture( std::shared_ptr<class PureConceptImage> Img );
    Texture( const std::string& TexturePath );
    Texture( ) = default;
    ~Texture( );

    void
    SetTexturePath( const std::string& TexturePath );

    void
    SetImage( std::shared_ptr<class PureConceptImage> Image );

    /*
     *
     *
     *
     * */
    void
    LoadTexture( );

    void
    BindTexture( int BindAt = 0 ) const;

    uint32_t
    GetTextureID( ) const noexcept;

protected:
    uint32_t    m_TextureID = 0;
    std::string m_TexturePath;

    std::shared_ptr<class PureConceptImage> m_TextureImage;

    ENABLE_IMGUI( Texture )
};
