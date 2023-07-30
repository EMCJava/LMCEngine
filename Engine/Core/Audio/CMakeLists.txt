add_library(Audio.lib Audio.hpp Audio.cpp)

add_library(AudioEngine.lib AudioEngine.hpp AudioEngine.cpp)

target_link_libraries(AudioEngine.lib audio.base.lib spdlog::spdlog)
target_link_libraries(Audio.lib AudioEngine.lib)