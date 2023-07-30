include(FetchContent)

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

if (WIN32)
    set_target_properties(irrklang PROPERTIES IMPORTED_IMPLIB ${irrklang_SOURCE_DIR}/lib/Winx64-visualStudio/audio.base.lib)
    set_target_properties(irrklang PROPERTIES IMPORTED_LOCATION ${irrklang_SOURCE_DIR}/bin/winx64-visualStudio/irrKlang.dll)

    add_custom_target(
            populate_irrklang
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${irrklang_SOURCE_DIR}/bin/winx64-visualStudio/irrKlang.dll ${CMAKE_BINARY_DIR}/.
    )
elseif (APPLE)
    # message(FATAL_ERROR "CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES}")

    EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
    if (${ARCHITECTURE} STREQUAL "arm64")
        message("Not support for arm64 on apple")

        set(no_irrklang)
    else ()
        set_target_properties(irrklang PROPERTIES IMPORTED_LOCATION ${irrklang_SOURCE_DIR}/bin/macosx-gcc/libirrklang.dylib)
    endif ()

    add_custom_target(populate_irrklang)
elseif (UNIX)
    message(FATAL_ERROR "Platform not tested")
else ()
    message(FATAL_ERROR "No supported platform was detected")
endif ()

add_library(audio.base.lib INTERFACE)

if (NOT ${no_irrklang})

    add_library(audio.base.lib INTERFACE)
    target_link_libraries(audio.base.lib INTERFACE irrklang)
    add_dependencies(audio.base.lib populate_irrklang)

    target_compile_definitions(audio.base.lib INTERFACE USE_IRRKLANG)

else ()
    target_compile_definitions(audio.base.lib INTERFACE NO_AUDIO_ENGINE)
endif ()