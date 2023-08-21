declare_library(
        NAME
        Map.TileSpriteSet

        SRCS
        Map/Private/TileSpriteSet.cpp

        P_DEPS
        SpriteSquareTexture.lib
        Engine.lib

        DEPS
        OrientationCoordinate.lib

        STATIC
)

declare_library(
        NAME
        Effect.ToleranceVisualizer

        SRCS
        Effect/Private/ToleranceVisualizer.cpp

        P_DEPS
        HotReloadFrameBuffer.lib
        SpriteSquareTexture.lib
        spdlog::spdlog

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
        spdlog::spdlog

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
        spdlog::spdlog

        DEPS

        STATIC
)

declare_library(
        NAME
        Map.SongMapControl

        SRCS
        Map/Private/SongMapControl.cpp

        P_DEPS
        Map.TileSpriteSet.lib
        spdlog::spdlog

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
        Map.SongMapControl.lib
        SpriteSquareAnimatedTexture.lib
        Effect.ToleranceVisualizer.lib
        Effect.ToleranceBar.lib

        DEPS
        ConceptApplicable.lib
        OrientationCoordinate.lib
        spdlog::spdlog
        Audio.lib
)