cmake_minimum_required(VERSION 3.25)
project(ADanceOfFireAndIce)
set(CMAKE_CXX_STANDARD 23)

## ---------- Engine setting ----------
set(ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(EditorBuild true)

## ---------- Game setting ----------
set(GAME_CONFIG_PATH ADanceOfFireAndIce.lmce)
message("Game project config file ${GAME_CONFIG_PATH}")

## ---------- CMake module ----------
list(APPEND CMAKE_MODULE_PATH "${ENGINE_ROOT}/cmake")
include(functions)
include(defines)

## ---------- Shared source (Hot reload) setting ----------
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SHARED_BINARY_PATH_FORMAT "${RELATIVE_BINARY_DIR}/Source/${CMAKE_SHARED_LIBRARY_PREFIX}{}.lib${CMAKE_SHARED_LIBRARY_SUFFIX}")

## Write hot reload config in the project config file
file(READ ${GAME_CONFIG_PATH} project_config_data)
string(JSON project_new_config_data SET "${project_config_data}" "shared_library_path_${CMAKE_BUILD_TYPE}_format" "\"${SHARED_BINARY_PATH_FORMAT}\"")
message(${project_new_config_data})
file(WRITE ${GAME_CONFIG_PATH} "${project_new_config_data}")

## ---------- Engine setup ----------
include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

## ---------- Game executable ----------
add_subdirectory(Source)

add_executable(ADanceOfFireAndIce main.cpp)
target_link_libraries(ADanceOfFireAndIce Engine.lib)

# Make sure game source changes will compile
target_link_libraries(ADanceOfFireAndIce Foo.CFoo.lib)
