add_library(RayCast.lib RayCast.hpp RayCast.cpp)

target_link_libraries(RayCast.lib glm::glm PhysX::PhysX Engine.lib PhysicsEngine.lib PureConceptPerspectiveCamera.lib)