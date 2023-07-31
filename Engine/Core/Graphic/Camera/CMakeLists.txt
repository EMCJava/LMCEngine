add_library(PureConceptCamera.lib PureConceptCamera.hpp PureConceptCamera.cpp)

target_link_libraries(PureConceptCamera.lib glm::glm PureConcept.lib)