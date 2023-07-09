cmake_minimum_required(VERSION 3.25)
project(ADanceOfFireAndIce)

set(CMAKE_CXX_STANDARD 23)

set(EditorBuild true)

if (${EditorBuild})
    add_compile_definitions(LMC_API_EXPORTS)
else ()
    add_compile_definitions(LMC_API_STATIC)
endif ()

function(declare_library target_name)
    if (${EditorBuild})
        add_library(${target_name} SHARED ${ARGN})
    else ()
        add_library(${target_name} STATIC ${ARGN})
    endif ()
endfunction()


set(ENGINE_ROOT ../../)

include_directories(${ENGINE_ROOT})
add_subdirectory(${ENGINE_ROOT}/Engine Engine.Binary)

add_subdirectory(Source)

add_executable(ADanceOfFireAndIce main.cpp)

target_link_libraries(ADanceOfFireAndIce Engine.lib)
target_link_libraries(ADanceOfFireAndIce DynamicConcept.lib)