include_directories(Foo/Public)

declare_library(Foo.CFoo.lib Foo/Private/CFoo.cpp)
target_link_libraries(Foo.CFoo.lib Concept.lib)
target_link_libraries(Foo.CFoo.lib spdlog::spdlog)