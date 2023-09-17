declare_library(
        NAME
        GameManager.GameManager

        SRCS
        GameManager/Private/GameManager.cpp

        P_DEPS
        Engine.lib


        DEPS
        spdlog::spdlog
)