add_library(DynamicLibrary.lib DynamicLibrary.hpp DynamicLibrary.cpp)
add_library(DynamicConcept.lib DynamicConcept.hpp DynamicConcept.cpp)

target_link_libraries(DynamicLibrary.lib spdlog::spdlog)
target_link_libraries(DynamicConcept.lib DynamicLibrary.lib)