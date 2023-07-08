cmake_minimum_required(VERSION 3.25)
project(ADanceOfFireAndIce)

set(CMAKE_CXX_STANDARD 23)

set(ENGINE_ROOT ../../)

include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

add_subdirectory(Source)

add_executable(ADanceOfFireAndIce main.cpp)

target_link_libraries(ADanceOfFireAndIce Engine.lib)
