add_library(Window.lib Window.hpp GLFWWindow.hpp  GLFWWindow.cpp)
add_library(WindowPool.lib WindowPool.hpp  WindowPool.cpp)

target_link_libraries(Window.lib PRIVATE imgui.lib glfw glad.lib)
target_link_libraries(WindowPool.lib PRIVATE imgui.lib Window.lib glfw)