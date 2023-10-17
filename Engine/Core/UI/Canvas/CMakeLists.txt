add_library(Canvas.lib Canvas.hpp Canvas.cpp)

target_link_libraries(Canvas.lib ConceptRenderable.lib PureConceptCamera.lib)