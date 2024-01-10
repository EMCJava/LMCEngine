# For shared library path finding
# From https://searchcode.com/file/633126120/CMakeLists.txt/

set(LIB_RELATIVE_TO_RUNTIME /libs/)

if (APPLE)
    # always use our rpath
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # the / is required by some older versions of OS X
    set(CMAKE_INSTALL_RPATH "@executable_path/${LIB_RELATIVE_TO_RUNTIME}")
    set(CMAKE_MACOSX_RPATH TRUE)
elseif (NOT WIN32)
    # always use our rpath
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN${LIB_RELATIVE_TO_RUNTIME}")
endif ()

if (${EditorBuild})
    add_compile_definitions(LMC_EDITOR)
    add_compile_definitions(HOT_RELOAD)
    add_compile_definitions(LMC_API_EXPORTS)
    add_compile_definitions("PrimaryWindow=EditorWindow")
    add_compile_definitions("RootConceptTy=DynamicConcept")
    add_compile_definitions("RootConceptIncludePath=\"Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp\"")
else ()
    add_compile_definitions(LMC_API_STATIC)
    add_compile_definitions("PrimaryWindow=GameWindow")
    add_compile_definitions("RootConceptTy=${ROOT_CONCEPT_CLASS}")
    add_compile_definitions("RootConceptIncludePath=\"${ROOT_CONCEPT_PATH}\"")
endif ()

add_compile_definitions("GAME_CONFIG_PATH=\"${GAME_CONFIG_PATH}\"")
add_compile_definitions("CMAKE_BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\"")

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif ()

## For spdlog
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)

    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}${LIB_RELATIVE_TO_RUNTIME})
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    else ()
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}${LIB_RELATIVE_TO_RUNTIME})
    endif ()
else ()
    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        message(FATAL_ERROR "CMAKE_RUNTIME_OUTPUT_DIRECTORY not set while CMAKE_LIBRARY_OUTPUT_DIRECTORY set to${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        "Can't guarantee ${LIB_RELATIVE_TO_RUNTIME}:${LIB_RELATIVE_TO_RUNTIME}")
    endif ()
endif ()

message(
        STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)
message(
        STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)