add_library(Sprite.lib Sprite.hpp Sprite.cpp)
add_library(SpriteSquare.lib SpriteSquare.hpp SpriteSquare.cpp)
add_library(SpriteSquareTexture.lib SpriteSquareTexture.hpp SpriteSquareTexture.cpp)
add_library(SpriteSquareAnimatedTexture.lib SpriteSquareAnimatedTexture.hpp SpriteSquareAnimatedTexture.cpp)

add_subdirectory(Particle)

target_link_libraries(Sprite.lib Engine.lib Shader.lib PureConceptCamera.lib glm::glm OrientationMatrix.lib)
target_link_libraries(SpriteSquare.lib Sprite.lib)
target_link_libraries(SpriteSquareTexture.lib SpriteSquare.lib PureConceptImage.lib)
target_link_libraries(SpriteSquareAnimatedTexture.lib SpriteSquareTexture.lib)
