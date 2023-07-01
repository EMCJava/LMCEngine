add_library(Window.lib Window.hpp GLFWWindow.hpp  GLFWWindow.cpp)

target_link_libraries(Window.lib glfw)