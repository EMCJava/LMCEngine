cmake_minimum_required(VERSION 3.25)
project(LMCEngine)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_subdirectory(Engine)

add_executable(LMCEngine main.cpp)

target_link_libraries(LMCEngine Engine.lib)
