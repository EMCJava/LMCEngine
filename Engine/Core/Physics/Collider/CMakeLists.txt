add_subdirectory(Serializer)

add_library(PureConceptCollider.lib PureConceptCollider.hpp PureConceptCollider.cpp)
add_library(ColliderMesh.lib ColliderMesh.hpp ColliderMesh.cpp)

target_link_libraries(PureConceptCollider.lib PhysicsEngine.lib PureConcept.lib)
target_link_libraries(ColliderMesh.lib PureConceptCollider.lib ColliderSerializerGroupMesh.lib PhysicsScene.lib)