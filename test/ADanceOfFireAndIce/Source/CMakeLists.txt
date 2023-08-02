include_directories(Foo/Public)
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
        Foo.CFoo

        SRCS
        Foo/Private/CFoo.cpp

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