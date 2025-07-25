//
// Created by samsa on 9/10/2023.
//

#pragma once

#include <Engine/Core/Graphic/Sprites/Sprite.hpp>

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
    SetCenterAt( const glm::vec3& Coordinate );

    void
    SetFont( std::shared_ptr<class Font> Font ) { m_Font = Font; }

    void
    SetScale( FloatTy Scale ) { m_Scale = Scale; }

    void
    SetColor( const glm::vec3& Color ) { m_Color = Color; }

    uint32_t
    GetTextPixelWidth( );

    uint32_t
    GetTextPixelHeight( );

    auto&       GetText( ) { return m_Text; }
    const auto& GetText( ) const noexcept { return m_Text; }

protected:
    std::string m_Text;
    FloatTy     m_Scale { 1 };
    glm::vec3   m_TextCoordinate;
    glm::vec3   m_Color { 0 };

    std::shared_ptr<class Font> m_Font;

    ENABLE_IMGUI( Text )
};
