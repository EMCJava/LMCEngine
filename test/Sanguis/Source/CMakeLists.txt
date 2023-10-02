declare_library(
        NAME
        Witchcraft.Effect

        SRCS
        Witchcraft/Private/Effect.cpp

        P_DEPS
        Engine.lib

        DEPS
        spdlog::spdlog

        STATIC
)

declare_library(
        NAME
        Witchcraft.EffectMixer

        SRCS
        Witchcraft/Private/EffectMixer.cpp

        P_DEPS
        Engine.lib


        DEPS
        Witchcraft.Effect.lib
        spdlog::spdlog

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNode

        SRCS
        Witchcraft/Private/ControlNode.cpp

        P_DEPS
        Engine.lib

        DEPS
        spdlog::spdlog

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNodeSimpleEffect

        SRCS
        Witchcraft/Private/ControlNodeSimpleEffect.cpp

        P_DEPS
        Engine.lib

        DEPS
        Witchcraft.ControlNode.lib
        spdlog::spdlog

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNodePlaceholder

        SRCS
        Witchcraft/Private/ControlNodePlaceholder.cpp

        P_DEPS
        Engine.lib

        DEPS
        Witchcraft.ControlNode.lib
        spdlog::spdlog

        STATIC
)

declare_library(
        NAME
        Witchcraft.BaseBoard

        SRCS
        Witchcraft/Private/BaseBoard.cpp

        P_DEPS
        Engine.lib
        Witchcraft.EffectMixer.lib
        Witchcraft.ControlNode.lib
        Witchcraft.ControlNodeSimpleEffect.lib
        Witchcraft.ControlNodePlaceholder.lib
        yaml-cpp::yaml-cpp

        DEPS
        Witchcraft.Effect.lib
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
        Witchcraft.BaseBoard.lib
        ParticlePool.lib
        SpriteSquareTexture.lib
        ParticleAttributesRandomizer.lib
        PureConceptAABBSquare.lib
        UserInput.lib
        Orientation.lib

        DEPS
        spdlog::spdlog
)