add_subdirectory(Font)
add_subdirectory(Text)

add_library(RectButton.lib RectButton.hpp RectButton.cpp)

target_link_libraries(RectButton.lib Text.lib SpriteSquare.lib ConceptApplicable.lib PureConceptAABBBox.lib GlobalResourcePool.lib UserInput.lib PureConceptCamera.lib)