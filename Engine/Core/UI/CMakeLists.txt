add_library(RectButton.lib RectButton.hpp RectButton.cpp)

target_link_libraries(RectButton.lib SpriteSquare.lib ConceptApplicable.lib PureConceptAABBBox.lib GlobalResourcePool.lib UserInput.lib PureConceptCamera.lib)