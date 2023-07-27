add_library(Shader.lib Shader.hpp Shader.cpp)
add_library(ShaderProgram.lib ShaderProgram.hpp ShaderProgram.cpp)

target_link_libraries(ShaderProgram.lib PRIVATE spdlog::spdlog)
target_link_libraries(Shader.lib PRIVATE spdlog::spdlog glm::glm ShaderProgram.lib)
