add_library(Material.lib Material.hpp Material.cpp)

target_link_libraries(Material.lib Texture.lib Shader.lib)