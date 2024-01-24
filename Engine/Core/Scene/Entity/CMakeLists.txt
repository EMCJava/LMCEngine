add_library(Entity.lib Entity.hpp Entity.cpp)
add_library(EntityRenderable.lib EntityRenderable.hpp EntityRenderable.cpp)

target_link_libraries(Entity.lib ConceptList.lib OrientationMatrix.lib)
target_link_libraries(EntityRenderable.lib Entity.lib ConceptRenderable.lib)
