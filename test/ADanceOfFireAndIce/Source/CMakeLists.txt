include_directories(Foo/Public)

declare_library(
        NAME
        Foo.CFoo

        SRCS
        Foo/Private/CFoo.cpp

        P_DEPS
        Engine.lib

        DEPS
        Concept.lib
        spdlog::spdlog
)