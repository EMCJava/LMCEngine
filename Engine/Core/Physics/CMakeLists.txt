add_library(PhysicsEngine.lib PhysicsEngine.hpp PhysicsEngine.cpp)
add_library(PhysicsScene.lib PhysicsScene.hpp PhysicsScene.cpp)

add_subdirectory(Collider)
add_subdirectory(RigidBody)
add_subdirectory(Controller)
add_subdirectory(Queries)

target_link_libraries(PhysicsScene.lib PhysX::PhysX spdlog.lib)
target_link_libraries(PhysicsEngine.lib PhysicsScene.lib)