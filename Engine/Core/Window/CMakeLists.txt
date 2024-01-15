add_library(Window.lib Window.hpp GLFWWindow.hpp GLFWWindow.cpp)
add_library(EditorWindow.lib EditorWindow.hpp EditorWindow.cpp)
add_library(GameWindow.lib GameWindow.hpp GameWindow.cpp)
add_library(WindowPool.lib WindowPool.hpp WindowPool.cpp)

target_link_libraries(Window.lib PRIVATE glfw.lib spdlog.lib glad.lib)
target_link_libraries(GameWindow.lib PRIVATE Window.lib glfw.lib glad.lib spdlog.lib glm::glm PureConcept.lib)
target_link_libraries(EditorWindow.lib PRIVATE GameWindow.lib PhysX::PhysX imgui.lib imguizmo.lib spdlog.lib OSFile.lib Project.lib glm::glm imgui_club libexecstream.lib ConceptCoreRuntime.lib RigidBody.lib PureConceptPerspectiveCamera.lib)
target_link_libraries(WindowPool.lib PRIVATE Window.lib glfw.lib)
