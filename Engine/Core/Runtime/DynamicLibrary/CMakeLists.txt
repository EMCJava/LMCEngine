add_library(DynamicLibrary.lib DynamicLibrary.hpp DynamicLibrary.cpp)
target_link_libraries(DynamicLibrary.lib spdlog::spdlog)