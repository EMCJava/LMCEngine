add_library(UserInput.lib UserInput.hpp UserInput.cpp)

target_link_libraries(UserInput.lib glfw spdlog::spdlog)