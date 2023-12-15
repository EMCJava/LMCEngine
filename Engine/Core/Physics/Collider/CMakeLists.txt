add_subdirectory(Serializer)

add_library(Collider.lib Collider.hpp Collider.cpp)
add_library(ColliderMesh.lib ColliderMesh.hpp ColliderMesh.cpp)

target_link_libraries(Collider.lib PhysicsEngine.lib ConceptList.lib)
target_link_libraries(ColliderMesh.lib Collider.lib ColliderSerializerGroupMesh.lib PhysicsScene.lib RenderableMeshHitBox.lib)