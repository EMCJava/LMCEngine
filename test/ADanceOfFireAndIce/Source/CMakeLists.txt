declare_library(
        NAME
        Map.TileSpriteSet

        SRCS
        Map/Private/TileSpriteSet.cpp

        P_DEPS
        SpriteSquareTexture.lib
        Engine.lib

        DEPS

        STATIC
)

declare_library(
        NAME
        Effect.ToleranceBarStrike

        SRCS
        Effect/Private/ToleranceBarStrike.cpp

        P_DEPS
        SpriteSquareTexture.lib
        spdlog.lib

        DEPS

        STATIC
)

declare_library(
        NAME
        Effect.ToleranceBar

        SRCS
        Effect/Private/ToleranceBar.cpp

        P_DEPS
        SpriteSquareTexture.lib
        Effect.ToleranceBarStrike.lib
        PureConceptImage.lib
        spdlog.lib

        DEPS

        STATIC
)

declare_library(
        NAME
        GameManager.GameManager

        SRCS
        GameManager/Private/GameManager.cpp

        P_DEPS
        Engine.lib
        ConceptRenderable.lib
        glad.lib
        Shader.lib
        SpriteSquare.lib
        SpriteSquareTexture.lib
        ShaderProgram.lib
        PureConceptCamera.lib
        Map.TileSpriteSet.lib
        SpriteSquareAnimatedTexture.lib
        Effect.ToleranceBar.lib
        GlobalResourcePool.lib
        RectButton.lib
        PureConceptOrthoCamera.lib
        Canvas.lib

        DEPS
        ConceptApplicable.lib
        spdlog.lib
        Audio.lib
)