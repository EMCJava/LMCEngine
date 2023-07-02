add_library(Project.lib Project.hpp Project.cpp)

target_link_libraries(Project.lib spdlog::spdlog nlohmann_json::nlohmann_json)