add_library(SerializerModel.lib SerializerModel.hpp SerializerModel.cpp)

target_link_libraries(SerializerModel.lib assimp spdlog::spdlog ConceptMesh.lib ConceptRenderable.lib)