add_library(Window.lib Window.hpp GLFWWindow.hpp GLFWWindow.cpp)
add_library(EditorWindow.lib EditorWindow.hpp EditorWindow.cpp)
add_library(GameWindow.lib GameWindow.hpp GameWindow.cpp)
add_library(WindowPool.lib WindowPool.hpp WindowPool.cpp)

target_link_libraries(Window.lib PRIVATE glfw spdlog::spdlog glad.lib)
target_link_libraries(GameWindow.lib PRIVATE Window.lib glfw glad.lib spdlog::spdlog glm::glm PureConcept.lib)
target_link_libraries(EditorWindow.lib PRIVATE GameWindow.lib imgui.lib imgui.demo.lib spdlog::spdlog OSFile.lib Project.lib glm::glm imgui_club libexecstream.lib ConceptCoreRuntime.lib RigidBody.lib)
target_link_libraries(WindowPool.lib PRIVATE Window.lib glfw)
