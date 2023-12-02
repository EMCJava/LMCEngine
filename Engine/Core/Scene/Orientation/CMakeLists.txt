add_library(Orientation.lib Orientation.hpp Orientation.cpp)

target_link_libraries(Orientation.lib spdlog::spdlog glm::glm imgui.lib ConceptCoreRuntime.lib)