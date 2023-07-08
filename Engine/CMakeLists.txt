find_package(glfw3 REQUIRED)

add_subdirectory(Library)
include_directories(Library)

add_subdirectory(Core)

add_library(Engine.lib Engine.hpp Engine.cpp)

target_link_libraries(Engine.lib Environment.lib WindowPool.lib EditorWindow.lib GameWindow.lib spdlog::spdlog glad.lib glfw imgui.lib)