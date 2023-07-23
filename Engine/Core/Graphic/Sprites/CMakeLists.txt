add_library(Sprite.lib Sprite.hpp Sprite.cpp)
add_library(SpriteSquare.lib SpriteSquare.hpp SpriteSquare.cpp)
add_library(SpriteSquareTexture.lib SpriteSquareTexture.hpp SpriteSquareTexture.cpp)

target_link_libraries(Sprite.lib Engine.lib Shader.lib Engine.lib glm::glm)
target_link_libraries(SpriteSquare.lib Sprite.lib)
target_link_libraries(SpriteSquareTexture.lib SpriteSquare.lib stb)
