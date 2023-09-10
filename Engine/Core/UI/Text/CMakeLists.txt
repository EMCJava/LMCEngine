add_library(Text.lib Text.hpp Text.cpp)

target_link_libraries(Text.lib Font.lib spdlog::spdlog glm::glm imgui.lib)
