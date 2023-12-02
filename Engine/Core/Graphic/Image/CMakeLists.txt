add_library(PureConceptImage.lib PureConceptImage.hpp PureConceptImage.cpp)
target_link_libraries(PureConceptImage.lib imgui.lib stb spdlog::spdlog ConceptCoreRuntime.lib
        #not good
        glm::glm
)
