add_library(Shader.lib Shader.hpp Shader.cpp)

target_link_libraries(Shader.lib PRIVATE spdlog::spdlog glm::glm)