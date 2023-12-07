add_library(Project.lib Project.hpp Project.cpp)

target_link_libraries(Project.lib spdlog.lib nlohmann_json::nlohmann_json)