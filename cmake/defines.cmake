# For shared library path finding
# From https://searchcode.com/file/633126120/CMakeLists.txt/
if (APPLE)
    # always use our rpath
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # the / is required by some older versions of OS X
    set(CMAKE_INSTALL_RPATH "@executable_path/")
    set(CMAKE_MACOSX_RPATH TRUE)
elseif (NOT WIN32)
    # always use our rpath
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN")
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