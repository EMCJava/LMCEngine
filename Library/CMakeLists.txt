include(FetchContent)

## -------------- IMGUI --------------
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG master
)

FetchContent_MakeAvailable(imgui)

file(GLOB imgui_source
        ${imgui_SOURCE_DIR}/*.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )

add_library(imgui.lib STATIC ${imgui_source})
target_include_directories(imgui.lib PUBLIC ${imgui_SOURCE_DIR})

target_link_libraries(imgui.lib glfw)

## -------------- GLAD --------------
add_library(glad.lib glad/gl.h glad/gl.c)
target_include_directories(glad.lib PRIVATE .)
