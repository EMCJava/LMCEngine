//
// Created by loyus on 8/21/2023.
//

#pragma once

#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Concept/ConceptSetCache.hpp>

class ToleranceBar : public SpriteSquareTexture
{
    DECLARE_CONCEPT( ToleranceBar, SpriteSquareTexture )

    void
    SetupBarTemplate( );

public:
    ToleranceBar( int Width, int Height );

    void
    Render( ) override;

    void
    SetupSprite( ) override;

    /*
     *
     * Location is in range [-1,1]
     * 0 is the center of the bar
     *
     * */
    void
    AddBar( FloatTy Location );

protected:
    std::shared_ptr<class PureConceptImage> m_BarImage;
    std::shared_ptr<class Shader>           m_BarShader;

    std::unique_ptr<class ToleranceBarStrike> m_ToleranceBarStrikeTemplate;
    ConceptSetFetchCache<class ToleranceBarStrike> m_ToleranceBarStrikeCache;

    std::pair<FloatTy, FloatTy> m_PreviousCameraSize;
    std::pair<int32_t, int32_t> m_BarSize;

    ENABLE_IMGUI( ToleranceBar )
};
