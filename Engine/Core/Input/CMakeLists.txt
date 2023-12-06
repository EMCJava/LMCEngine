add_library(UserInput.lib UserInput.hpp UserInput.cpp)
add_subdirectory(Serializer)

target_link_libraries(UserInput.lib glfw.lib spdlog::spdlog)