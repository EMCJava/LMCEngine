if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    message(
            STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
    )
endif ()

if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY OR NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    message(
            STATUS "No CMAKE_LIBRARY_OUTPUT_DIRECTORY selected, defaulting to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    )
endif ()

if (NOT DEFINED EditorBuild)
    set(EditorBuild true)
endif ()
message(STATUS "EditorBuild: ${EditorBuild}")

include(functions)
include(defines)
include(hot_reload)
