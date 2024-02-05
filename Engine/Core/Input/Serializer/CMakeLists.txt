add_library(SerializerModel.lib SerializerModel.hpp SerializerModel.cpp)

target_link_libraries(SerializerModel.lib

        PUBLIC

        assimp
        spdlog.lib
        ConceptRenderable.lib
        Material.lib
        RenderableMesh.lib
        RenderableMeshCluster.lib
)