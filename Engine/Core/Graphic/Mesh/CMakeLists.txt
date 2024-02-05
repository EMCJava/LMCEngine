add_library(ConceptMesh.lib ConceptMesh.hpp ConceptMesh.cpp)
add_library(RenderableMesh.lib RenderableMesh.hpp RenderableMesh.cpp)
add_library(RenderableMeshHitBox.lib RenderableMeshHitBox.hpp RenderableMeshHitBox.cpp)
add_library(RenderableMeshCluster.lib RenderableMeshCluster.hpp RenderableMeshCluster.cpp)

target_link_libraries(ConceptMesh.lib PureConcept.lib SerializerModel.lib)
target_link_libraries(RenderableMesh.lib ConceptRenderable.lib ConceptMesh.lib Material.lib)
target_link_libraries(RenderableMeshHitBox.lib RenderableMesh.lib)
target_link_libraries(RenderableMeshCluster.lib RenderableMesh.lib Material.lib)