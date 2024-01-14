add_library(Orientation.lib Orientation.hpp Orientation.cpp)
add_library(OrientationMatrix.lib OrientationMatrix.hpp OrientationMatrix.cpp)

target_link_libraries(Orientation.lib spdlog.lib glm::glm ConceptCoreRuntime.lib)
target_link_libraries(OrientationMatrix.lib Orientation.lib imgui.lib)