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

        DEPS
        ConceptApplicable.lib
        OrientationCoordinate.lib
        spdlog::spdlog
        Audio.lib
)