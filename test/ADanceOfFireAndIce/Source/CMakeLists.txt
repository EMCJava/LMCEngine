include_directories(Foo/Public)

declare_library(
        NAME
        Foo.CFoo

        SRCS
        Foo/Private/CFoo.cpp

        DEPS
        Concept.lib
        spdlog::spdlog
)