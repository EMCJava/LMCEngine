## -------------- PhysX --------------
find_package(PhysX REQUIRED)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SUFFIX "debug/bin")
else()
    set(SUFFIX "bin")
endif()

add_custom_target(populate_physx)
add_custom_command(TARGET populate_physx PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${SUFFIX}/*"
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
add_dependencies(PhysX::PhysX populate_physx)

add_subdirectory(Library)
include_directories(Library)

add_subdirectory(Core)

add_library(Engine.lib Engine.hpp Engine.cpp)

target_link_libraries(Engine.lib
        Environment.lib
        WindowPool.lib
        EditorWindow.lib
        GameWindow.lib
        spdlog.lib
        glad.lib
        glfw.lib
        imgui.lib
        DynamicConcept.lib
        ConceptApplicable.lib
        HotReloadFrameBuffer.lib
        ConceptRenderable.lib
        ConceptSetCache.lib
        Audio.lib
        UserInput.lib
        PureConceptCamera.lib
        PureConceptCameraStack.lib
        ConceptCoreRuntime.lib
        GlobalResourcePool.lib
        ShaderProgram.lib
        Shader.lib
        Font.lib
        PhysicsEngine.lib
        PhysicsScene.lib

        SpriteSquare.lib
)

add_subdirectory(Test)