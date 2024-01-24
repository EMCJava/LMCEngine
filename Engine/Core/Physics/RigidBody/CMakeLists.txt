add_library(RigidBody.lib RigidBody.hpp RigidBody.cpp)
add_library(RigidMesh.lib RigidMesh.hpp RigidMesh.cpp)

target_link_libraries(RigidBody.lib ConceptApplicable.lib ConceptRenderable.lib Collider.lib OrientationMatrix.lib Entity.lib)
target_link_libraries(RigidMesh.lib RigidBody.lib SerializerModel.lib RenderableMesh.lib EntityRenderable.lib)