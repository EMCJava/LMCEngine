add_subdirectory(Font)
add_subdirectory(Text)
add_subdirectory(Canvas)

add_library(RectButton.lib RectButton.hpp RectButton.cpp)

target_link_libraries(RectButton.lib Text.lib SpriteSquare.lib ConceptApplicable.lib PureConceptAABBSquare.lib GlobalResourcePool.lib UserInput.lib PureConceptCamera.lib)