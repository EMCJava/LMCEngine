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

## -------------- GLAD --------------
add_library(glad.lib glad/gl.h glad/gl.c)
target_include_directories(glad.lib PRIVATE .)

## -------------- nativefiledialog --------------
FetchContent_Declare(
        nativefiledialog
        GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
        GIT_TAG master
)

FetchContent_MakeAvailable(nativefiledialog)

file(GLOB nativefiledialog_source
        ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
        ${nativefiledialog_SOURCE_DIR}/src/nfd_win.cpp
        )

add_library(nativefiledialog.lib STATIC ${nativefiledialog_source})
target_include_directories(nativefiledialog.lib PUBLIC ${nativefiledialog_SOURCE_DIR}/src/include)


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
