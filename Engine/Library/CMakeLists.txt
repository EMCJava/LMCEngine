include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

## -------------- GLFW --------------
find_package(glfw3 CONFIG REQUIRED)

add_library(glfw.lib INTERFACE)
target_link_libraries(glfw.lib INTERFACE glfw)

## -------------- IMGUI --------------
find_package(imgui CONFIG REQUIRED)

add_library(imgui.lib INTERFACE)
target_link_libraries(imgui.lib INTERFACE imgui::imgui)
target_link_libraries(imgui.lib INTERFACE glfw.lib)

## -------------- ImGuizmo --------------
find_package(imguizmo CONFIG REQUIRED)

add_library(imguizmo.lib INTERFACE)
target_link_libraries(imguizmo.lib INTERFACE imguizmo::imguizmo)
target_link_libraries(imguizmo.lib INTERFACE imgui.lib)

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
find_package(yaml-cpp CONFIG REQUIRED)

add_library(yaml.lib INTERFACE)
target_link_libraries(yaml.lib INTERFACE yaml-cpp::yaml-cpp)

## -------------- spdlog --------------
find_package(spdlog CONFIG REQUIRED)

add_library(spdlog.lib INTERFACE)
target_link_libraries(spdlog.lib INTERFACE spdlog::spdlog)

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
    set_target_properties(irrklang PROPERTIES 
        IMPORTED_LOCATION ${irrklang_SOURCE_DIR}/bin/linux-gcc-64/libIrrKlang.so
        IMPORTED_NO_SONAME TRUE)

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
