add_library(SerializerModel.lib SerializerModel.hpp SerializerModel.cpp)

target_link_libraries(SerializerModel.lib assimp spdlog.lib ConceptMesh.lib ConceptRenderable.lib)