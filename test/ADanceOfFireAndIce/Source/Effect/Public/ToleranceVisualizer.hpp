//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

class ToleranceVisualizer : public SpriteSquareTexture
{
    DECLARE_CONCEPT( ToleranceVisualizer, SpriteSquareTexture )

    void
    SetupFadeShader( );

    void
    SetupBarShader( const std::string& BarTexturePath );

public:
    ToleranceVisualizer( int Width, int Height, const std::string& BarTexturePath );

    void
    Render( ) override;

protected:
    std::shared_ptr<class Shader> m_FadeShader;
    std::shared_ptr<class Shader> m_BarShader;

    std::unique_ptr<class HotReloadFrameBuffer> m_PassFrameBuffer;

    ENABLE_IMGUI( ToleranceVisualizer )
};