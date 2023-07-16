add_library(Sprite.lib Sprite.hpp Sprite.cpp)
add_library(SpriteSquare.lib SpriteSquare.hpp SpriteSquare.cpp)

target_link_libraries(Sprite.lib Engine.lib)
target_link_libraries(SpriteSquare.lib Sprite.lib)