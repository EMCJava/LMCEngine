add_library(ConceptMesh.lib ConceptMesh.hpp ConceptMesh.cpp)
add_library(RenderableMesh.lib RenderableMesh.hpp RenderableMesh.cpp)

target_link_libraries(ConceptMesh.lib PureConcept.lib)
target_link_libraries(RenderableMesh.lib ConceptRenderable.lib ConceptMesh.lib)