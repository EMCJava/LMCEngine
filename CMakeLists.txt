cmake_minimum_required(VERSION 3.25)
project(LMCEngine)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 REQUIRED)

include_directories(.)
include_directories(Library)

add_subdirectory(Engine)

add_executable(LMCEngine main.cpp)

target_link_libraries(LMCEngine glfw)
target_link_libraries(LMCEngine Engine.lib)
