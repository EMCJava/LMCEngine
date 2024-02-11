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