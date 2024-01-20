add_library(PhyController.lib PhyController.hpp PhyController.cpp)
add_library(EntityPhyController.lib EntityPhyController.hpp EntityPhyController.cpp)

target_link_libraries(PhyController.lib PhysX::PhysX glm::glm Engine.lib PhysicsEngine.lib spdlog.lib)
target_link_libraries(EntityPhyController.lib PhyController.lib)
