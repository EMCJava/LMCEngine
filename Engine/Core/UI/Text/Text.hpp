//
// Created by samsa on 9/10/2023.
//

#pragma once

#include <Engine/Core/Graphic/Sprites/Sprite.hpp>
#include <Engine/Core/Scene/Orientation/OrientationCoordinate.hpp>

#include <memory>
#include <string>

class Text : public Sprite
{
    DECLARE_CONCEPT( Text, Sprite )

public:
    Text( const std::string& Text = "" );

    virtual void
    SetupSprite( );

    virtual void
    Render( );

    void
    SetCoordinate( const auto& Coordinate ) { m_TextCoordinate = Coordinate; }

    void
    SetFont( std::shared_ptr<class Font> Font ) { m_Font = Font; }

protected:
    std::string                       m_Text;
    FloatTy                           m_Scale { 1 };
    OrientationCoordinate::Coordinate m_TextCoordinate;

    std::shared_ptr<class Font> m_Font;
};
