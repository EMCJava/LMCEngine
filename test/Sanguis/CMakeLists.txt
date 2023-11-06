cmake_minimum_required(VERSION 3.25)
project(Sanguis)
set(CMAKE_CXX_STANDARD 23)

## ---------- Project setting ----------
set(GAME_CONFIG_PATH Sanguis.lmce)
set(ROOT_CONCEPT_CLASS GameManager)
set(ROOT_CONCEPT_PATH ${CMAKE_SOURCE_DIR}/Source/GameManager/Public/GameManager.hpp)
set(ROOT_CONCEPT_LIB GameManager.GameManager.lib)
set(PROJECT_PATH_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ProjectCache/ProjectPath.config)

## ---------- Engine setting ----------
set(ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

## ---------- CMake module ----------
list(APPEND CMAKE_MODULE_PATH "${ENGINE_ROOT}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Source/cmake")
include(editor)
include(SourceInclude)

## ---------- Engine setup ----------
include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

## ---------- Game executable ----------
add_subdirectory(Source)
declare_main_executable(Sanguis)

find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
target_link_libraries(Sanguis unofficial::omniverse-physx-sdk::sdk)