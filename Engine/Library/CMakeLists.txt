include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

## -------------- IMGUI --------------
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

file(GLOB imgui_source
        ${imgui_SOURCE_DIR}/*.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui.lib STATIC ${imgui_source})
target_include_directories(imgui.lib PUBLIC ${imgui_SOURCE_DIR})

add_library(imgui.demo.lib STATIC ${imgui_SOURCE_DIR}/imgui_demo.cpp)

target_link_libraries(imgui.lib glfw)
target_link_libraries(imgui.demo.lib PRIVATE imgui.lib)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(imgui.lib imguizmo::imguizmo)

## -------------- GLFW --------------
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)

FetchContent_GetProperties(glfw)
if (NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif ()

## -------------- GLAD --------------
add_library(glad.lib glad/gl.h glad/gl.c)
target_include_directories(glad.lib PUBLIC .)

## -------------- libexecstream --------------
add_library(libexecstream.lib libexecstream/exec-stream.h libexecstream/exec-stream.cpp)
target_include_directories(libexecstream.lib PUBLIC .)

## -------------- nativefiledialog --------------
FetchContent_Declare(
        nativefiledialog
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG master
)

FetchContent_MakeAvailable(nativefiledialog)

if (APPLE)
    target_link_libraries(nfd PRIVATE "-framework AppKit" "-framework UniformTypeIdentifiers")
endif ()

## -------------- yaml-cpp --------------
FetchContent_Declare(
        cppyaml
        URL https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.zip
)

FetchContent_MakeAvailable(cppyaml)

## -------------- spdlog --------------
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.x
)

FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    message(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif ()

## -------------- json --------------
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG develop
)

FetchContent_MakeAvailable(json)

## -------------- stb --------------
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)

FetchContent_GetProperties(stb)
if (NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif ()

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
target_compile_definitions(stb INTERFACE STB_IMAGE_IMPLEMENTATION)

## -------------- glm --------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)

FetchContent_GetProperties(glm)
if (NOT glm_POPULATED)
    FetchContent_Populate(glm)
    set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif ()

## -------------- irrKlang --------------
FetchContent_Declare(
        irrklang
        URL https://www.ambiera.at/downloads/irrKlang-64bit-1.6.0.zip
)

FetchContent_MakeAvailable(irrklang)

message("irrKlang_SOURCE_DIR: ${irrklang_SOURCE_DIR}")

add_library(irrklang SHARED IMPORTED GLOBAL)
target_include_directories(irrklang INTERFACE ${irrklang_SOURCE_DIR}/include)
set_target_properties(irrklang PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${irrklang_SOURCE_DIR}/include)

set(no_irrklang FALSE)

if (WIN32)
    set_target_properties(irrklang PROPERTIES IMPORTED_IMPLIB ${irrklang_SOURCE_DIR}/lib/Winx64-visualStudio/irrKlang.lib)
    set_target_properties(irrklang PROPERTIES IMPORTED_LOCATION ${irrklang_SOURCE_DIR}/bin/winx64-visualStudio/irrKlang.dll)

    add_custom_target(populate_irrklang_folder
            ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    add_custom_target(
            populate_irrklang ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${irrklang_SOURCE_DIR}/bin/winx64-visualStudio/irrKlang.dll ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/.
            DEPENDS populate_irrklang_folder
            COMMENT "Exporting irrKlang.dll into build tree"
    )
elseif (APPLE)
    # message(FATAL_ERROR " CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES} ")

    EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
    if (${ARCHITECTURE} STREQUAL "arm64")
        message("Not support for arm64 on apple ")

        set(no_irrklang TRUE)
    else ()
        message("Not on arm64 apple (${ARCHITECTURE}), enabling irrklang")
        set_target_properties(irrklang PROPERTIES IMPORTED_LOCATION ${irrklang_SOURCE_DIR}/bin/macosx-gcc/libirrklang.dylib)
    endif ()

    add_custom_target(populate_irrklang)
elseif (UNIX)
    # set_target_properties(irrklang PROPERTIES IMPORTED_IMPLIB ${irrklang_SOURCE_DIR}/bin/linux-gcc-64/libIrrKlang.so)
    set_target_properties(irrklang PROPERTIES IMPORTED_LOCATION libIrrKlang.so)

    add_custom_target(
            populate_irrklang ALL
            COMMENT "Exporting libIrrKlang.so into build tree"
    )

    # file(COPY ${irrklang_SOURCE_DIR}/bin/linux-gcc-64/libIrrKlang.so DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    configure_file(${irrklang_SOURCE_DIR}/bin/linux-gcc-64/libIrrKlang.so ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} COPYONLY)
    if (NOT "${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
        # make sure on linux that libIrrKlang.so can be found when building
        configure_file(${irrklang_SOURCE_DIR}/bin/linux-gcc-64/libIrrKlang.so ${CMAKE_BINARY_DIR} COPYONLY)
    endif ()
else ()
    message(FATAL_ERROR "No supported platform was detected ")
endif ()

add_library(audio.base.lib INTERFACE)

if (NOT ${no_irrklang})

    target_link_libraries(audio.base.lib INTERFACE irrklang)
    add_dependencies(audio.base.lib populate_irrklang)

    target_compile_definitions(audio.base.lib INTERFACE USE_IRRKLANG)

    message("Using irrklang")

else ()
    target_compile_definitions(audio.base.lib INTERFACE NO_AUDIO_ENGINE)
endif ()

## -------------- imgui_memory_editor --------------
FetchContent_Declare(
        imgui_club
        GIT_REPOSITORY https://github.com/ocornut/imgui_club.git
        GIT_TAG main
        CONFIGURE_COMMAND ""
        BUILD_COMMAND " "
)

FetchContent_GetProperties(imgui_club)
if (NOT imgui_club_POPULATED)
    FetchContent_Populate(imgui_club)
endif ()

add_library(imgui_club INTERFACE)
target_include_directories(imgui_club INTERFACE ${imgui_club_SOURCE_DIR})


## -------------- freetype --------------
add_subdirectory(freetype-2.13.2)

## -------------- Catch2 --------------
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

## -------------- assimp --------------
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)
