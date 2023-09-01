add_library(Environment.lib Environment.hpp Environment.cpp)
add_library(GlobalResourcePool.lib GlobalResourcePool.hpp GlobalResourcePool.cpp)

target_link_libraries(Environment.lib Window.lib)
target_link_libraries(GlobalResourcePool.lib spdlog::spdlog)