add_subdirectory(Font)
add_subdirectory(Text)

add_library(RectButton.lib RectButton.hpp RectButton.cpp)
add_library(RectInput.lib RectInput.hpp RectInput.cpp)

target_link_libraries(RectButton.lib Text.lib SpriteSquare.lib ConceptList.lib PureConceptAABBSquare.lib GlobalResourcePool.lib UserInput.lib PureConceptCamera.lib)
target_link_libraries(RectInput.lib RectButton.lib)