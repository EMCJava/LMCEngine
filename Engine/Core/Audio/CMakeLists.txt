add_library(Audio.lib Audio.hpp Audio.cpp)

add_library(AudioEngine.lib AudioEngine.hpp AudioEngine.cpp)
add_library(AudioHandle.lib AudioHandle.hpp AudioHandle.cpp)

target_link_libraries(AudioHandle.lib audio.base.lib)
target_link_libraries(AudioEngine.lib AudioHandle.lib audio.base.lib spdlog.lib)
target_link_libraries(Audio.lib AudioEngine.lib)