add_library(PhysicsEngine.lib PhysicsEngine.hpp PhysicsEngine.cpp)
add_library(PhysicsScene.lib PhysicsScene.hpp PhysicsScene.cpp)

target_link_libraries(PhysicsScene.lib PhysX::PhysX spdlog::spdlog)
target_link_libraries(PhysicsEngine.lib PhysicsScene.lib)