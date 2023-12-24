add_library(Texture.lib Texture.hpp Texture.cpp)

target_link_libraries(Texture.lib imgui.lib ConceptCoreToImGuiImpl.lib ConceptCoreRuntime.lib)