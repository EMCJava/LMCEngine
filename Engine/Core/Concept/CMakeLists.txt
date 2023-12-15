add_library(ConceptCoreRuntime.lib INTERFACE ConceptCoreRuntime.hpp ConceptCoreRuntime.inl)
add_library(PureConcept.lib PureConcept.hpp PureConcept.cpp)
add_library(ConceptList.lib ConceptList.hpp ConceptList.cpp)
add_library(ConceptApplicable.lib ConceptApplicable.hpp ConceptApplicable.cpp)
add_library(ConceptRenderable.lib ConceptRenderable.hpp ConceptRenderable.cpp)
add_library(ConceptSetFetchCache.lib ConceptSetFetchCache.hpp ConceptFetchCache.cpp)

add_library(PureConceptAABB.lib PureConceptAABB.hpp PureConceptAABB.cpp)
add_library(PureConceptAABBSquare.lib PureConceptAABBSquare.hpp PureConceptAABBSquare.cpp)

target_link_libraries(PureConcept.lib Engine.lib ConceptCoreRuntime.lib imgui.lib)
target_link_libraries(PureConceptAABB.lib PureConcept.lib)
target_link_libraries(PureConceptAABBSquare.lib PureConceptAABB.lib)

target_link_libraries(ConceptList.lib ConceptSetFetchCache.lib PureConcept.lib)
target_link_libraries(ConceptApplicable.lib ConceptList.lib)
target_link_libraries(ConceptRenderable.lib ConceptList.lib)
target_link_libraries(ConceptCoreRuntime.lib INTERFACE spdlog.lib PureConcept.lib)
