add_library(Concept.lib Concept.hpp Concept.cpp)
add_library(ConceptApplicable.lib ConceptApplicable.hpp ConceptApplicable.cpp)
add_library(ConceptCoordinate.lib ConceptCoordinate.hpp ConceptCoordinate.cpp)

target_link_libraries(Concept.lib Engine.lib)
target_link_libraries(ConceptApplicable.lib Concept.lib)
target_link_libraries(ConceptCoordinate.lib Concept.lib)