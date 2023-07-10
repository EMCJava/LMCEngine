add_library(Concept.lib Concept.hpp Concept.cpp)
add_library(ConceptApplicable.lib ConceptApplicable.hpp ConceptApplicable.cpp)

target_link_libraries(Concept.lib Engine.lib)
target_link_libraries(ConceptApplicable.lib Concept.lib)
