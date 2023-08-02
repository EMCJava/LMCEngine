include_directories(GameManager/Public)
include_directories(Map/Public)

declare_library(
        NAME
        Map.TileSpriteSet

        SRCS
        Map/Private/TileSpriteSet.cpp

        P_DEPS
        SpriteSquareTexture.lib
        Engine.lib

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
        Audio.lib
        PureConceptCamera.lib
        Map.TileSpriteSet.lib

        DEPS
        ConceptApplicable.lib
        OrientationCoordinate.lib
        spdlog::spdlog
)