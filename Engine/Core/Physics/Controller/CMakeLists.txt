add_library(PhyController.lib PhyController.hpp PhyController.cpp)

target_link_libraries(PhyController.lib PhysX::PhysX glm::glm Engine.lib PhysicsEngine.lib spdlog.lib)