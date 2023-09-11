add_library(Font.lib Font.hpp Font.cpp)

target_link_libraries(Font.lib freetype spdlog::spdlog)