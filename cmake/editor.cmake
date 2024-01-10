if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
    message(
            STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}"
    )
endif ()

if (NOT DEFINED EditorBuild)
    set(EditorBuild true)
endif ()
message(STATUS "EditorBuild: ${EditorBuild}")

include(defines)
include(hot_reload)
include(functions)
