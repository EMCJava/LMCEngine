cmake_minimum_required(VERSION 3.25)
project(LMCEngine)
set(CMAKE_CXX_STANDARD 23)

## ---------- Engine setting ----------
set(ENGINE_ROOT ${CMAKE_SOURCE_DIR})
set(PURE_EDITOR_BUILD True)

## ---------- CMake module ----------
list(APPEND CMAKE_MODULE_PATH "${ENGINE_ROOT}/cmake")
include(editor)

## ---------- Engine setup ----------
include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

## ---------- Engine executable ----------
add_executable(LMCEngine main.cpp)
target_link_libraries(LMCEngine Engine.lib)