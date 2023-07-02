add_library(Window.lib Window.hpp GLFWWindow.hpp GLFWWindow.cpp)
add_library(EditorWindow.lib EditorWindow.hpp EditorWindow.cpp)
add_library(GameWindow.lib GameWindow.hpp GameWindow.cpp)
add_library(WindowPool.lib WindowPool.hpp WindowPool.cpp)

target_link_libraries(Window.lib PRIVATE glfw)
target_link_libraries(EditorWindow.lib PRIVATE Window.lib imgui.lib imgui.demo.lib spdlog::spdlog OSFile.lib)
target_link_libraries(GameWindow.lib PRIVATE Window.lib glfw glad.lib)
target_link_libraries(WindowPool.lib PRIVATE Window.lib glfw)
