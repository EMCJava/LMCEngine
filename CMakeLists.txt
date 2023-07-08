cmake_minimum_required(VERSION 3.25)
project(LMCEngine)

set(CMAKE_CXX_STANDARD 23)

set(ENGINE_ROOT .)

include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine)

add_executable(LMCEngine main.cpp)

target_link_libraries(LMCEngine Engine.lib)