add_library(PhyController.lib PhyController.hpp PhyController.cpp)
add_library(PhyControllerEntity.lib PhyControllerEntity.hpp PhyControllerEntity.cpp)
add_library(PhyControllerEntityPlayer.lib PhyControllerEntityPlayer.hpp PhyControllerEntityPlayer.cpp)

target_link_libraries(PhyController.lib PhysX::PhysX glm::glm Engine.lib PhysicsEngine.lib spdlog.lib)
target_link_libraries(PhyControllerEntity.lib PhyController.lib Entity.lib)
target_link_libraries(PhyControllerEntityPlayer.lib RigidBody.lib PhyControllerEntity.lib ConceptList.lib FirstPersonCameraController.lib Engine.lib UserInput.lib RayCast.lib)
