//
// Created by loyus on 8/21/2023.
//

#pragma once

#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>

class ToleranceBarStrike : public SpriteSquareTexture
{
    DECLARE_CONCEPT( ToleranceBarStrike, SpriteSquareTexture )

public:
    using SpriteSquareTexture::SpriteSquareTexture;

    void
    Render( ) override;

    ToleranceBarStrike&
    operator<<( ToleranceBarStrike& Other );

    void
    SetLifeTime( FloatTy LiftTime );

protected:
    FloatTy m_LifeTimeTotal { };
    FloatTy m_LifeTimeRemain { };

    ENABLE_IMGUI( ToleranceBarStrike )
};
