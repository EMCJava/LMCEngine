add_library(Text.lib Text.hpp Text.cpp)

target_link_libraries(Text.lib Font.lib spdlog.lib glm::glm imgui.lib)
