cmake_minimum_required(VERSION 3.25)
project(ADanceOfFireAndIce)
set(CMAKE_CXX_STANDARD 23)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    message(
            STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
    )
endif ()

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    message(
            STATUS "No CMAKE_RUNTIME_OUTPUT_DIRECTORY selected, defaulting to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
endif ()

## ---------- Project setting ----------
set(GAME_CONFIG_PATH ADanceOfFireAndIce.lmce)
add_compile_definitions("GAME_CONFIG_PATH=\"${GAME_CONFIG_PATH}\"")
set(ROOT_CONCEPT_CLASS GameManager)
set(ROOT_CONCEPT_PATH ${CMAKE_SOURCE_DIR}/Source/GameManager/Public/GameManager.hpp)
set(ROOT_CONCEPT_LIB GameManager.GameManager.lib)

## ---------- Engine setting ----------
set(ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(EditorBuild false)

## ---------- Game setting ----------
set(PROJECT_PATH_CONFIG_PATH ProjectCache/ProjectPath.config)
message("Game project config file ${GAME_CONFIG_PATH}")

## ---------- CMake module ----------
list(APPEND CMAKE_MODULE_PATH "${ENGINE_ROOT}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
include(functions)
include(defines)
include(SourceInclude)

## ---------- Shared source (Hot reload) setting ----------
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SHARED_BINARY_PATH_FORMAT "${RELATIVE_BINARY_DIR}/Source/${CMAKE_SHARED_LIBRARY_PREFIX}{}.lib${CMAKE_SHARED_LIBRARY_SUFFIX}")

## Write hot reload config in the project config file
file(READ ${PROJECT_PATH_CONFIG_PATH} project_config_data)
string(JSON project_new_config_data SET "${project_config_data}" "shared_library_path_${CMAKE_BUILD_TYPE}_format" "\"${SHARED_BINARY_PATH_FORMAT}\"")
message(${project_new_config_data})
file(WRITE ${PROJECT_PATH_CONFIG_PATH} "${project_new_config_data}")

## ---------- Engine setup ----------
include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

## ---------- Game executable ----------
add_subdirectory(Source)

add_executable(ADanceOfFireAndIce main.cpp)

if (NOT ${EditorBuild})
    target_link_libraries(GameWindow.lib PRIVATE ${ROOT_CONCEPT_LIB})
    target_link_libraries(Engine.lib ${ROOT_CONCEPT_LIB})
endif ()

target_link_libraries(ADanceOfFireAndIce ${ROOT_CONCEPT_LIB})
target_link_libraries(ADanceOfFireAndIce Engine.lib)