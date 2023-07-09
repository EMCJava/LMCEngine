add_library(Concept.lib Concept.hpp Concept.cpp)

target_link_libraries(Concept.lib PRIVATE Engine.lib)