cmake_minimum_required(VERSION 3.25)
project(ADanceOfFireAndIce)
set(CMAKE_CXX_STANDARD 23)

## ---------- Engine setting ----------
set(ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(EditorBuild true)

## ---------- CMake module ----------
list(APPEND CMAKE_MODULE_PATH "${ENGINE_ROOT}/cmake")
include(functions)
include(defines)

## ---------- Shared source setting ----------
add_compile_definitions("SHARED_BINARY_PATH_FORMAT=\"${CMAKE_CURRENT_BINARY_DIR}/Source/{}.lib${CMAKE_SHARED_LIBRARY_SUFFIX}\"")

## ---------- Engine setup ----------
include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

## ---------- Game executable ----------
add_subdirectory(Source)

add_executable(ADanceOfFireAndIce main.cpp)
target_link_libraries(ADanceOfFireAndIce Engine.lib)
