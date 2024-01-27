declare_library(
        NAME
        Witchcraft.Effect

        SRCS
        Witchcraft/Private/Effect.cpp

        P_DEPS
        Engine.lib

        DEPS
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Witchcraft.EffectMixer

        SRCS
        Witchcraft/Private/EffectMixer.cpp

        P_DEPS
        Engine.lib


        DEPS
        Witchcraft.Effect.lib
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNode

        SRCS
        Witchcraft/Private/ControlNode.cpp

        P_DEPS
        Engine.lib

        DEPS
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNodeSimpleEffect

        SRCS
        Witchcraft/Private/ControlNodeSimpleEffect.cpp

        P_DEPS
        Engine.lib

        DEPS
        Witchcraft.ControlNode.lib
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Witchcraft.ControlNodePlaceholder

        SRCS
        Witchcraft/Private/ControlNodePlaceholder.cpp

        P_DEPS
        Engine.lib

        DEPS
        Witchcraft.ControlNode.lib
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Witchcraft.BaseBoard

        SRCS
        Witchcraft/Private/BaseBoard.cpp

        P_DEPS
        Engine.lib
        Witchcraft.EffectMixer.lib
        Witchcraft.ControlNode.lib
        Witchcraft.ControlNodeSimpleEffect.lib
        Witchcraft.ControlNodePlaceholder.lib
        yaml.lib

        DEPS
        Witchcraft.Effect.lib
        spdlog.lib

        STATIC
)

declare_library(
        NAME
        Scenes.WandEditorScene

        SRCS
        Scenes/Private/WandEditorScene.cpp

        P_DEPS
        Witchcraft.BaseBoard.lib

        ParticlePool.lib
        PureConceptAABBSquare.lib
        ParticleAttributesRandomizer.lib
        PureConceptOrthoCamera.lib
        ConceptList.lib
        PureConceptImage.lib
        SpriteSquareTexture.lib
        UserInput.lib
        RectButton.lib
        Canvas.lib
        Engine.lib

        spdlog.lib

        DEPS

        STATIC
)

declare_library(
        NAME
        GameManager.GameManager

        SRCS
        GameManager/Private/GameManager.cpp

        P_DEPS
        Scenes.WandEditorScene.lib
        Engine.lib
        ConceptList.lib
        UserInput.lib
        Orientation.lib
        Canvas.lib
        PureConceptPerspectiveCamera.lib
        FirstPersonCameraController.lib
        ColliderMesh.lib
        RigidMesh.lib
        ColliderSerializerGroupMesh.lib
        SerializerModel.lib
        ConceptMesh.lib
        RenderableMesh.lib
        PhysicsEngine.lib
        PhysicsScene.lib
        Texture.lib
        Material.lib
        PhyControllerEntity.lib
        ConceptList.lib
        PhyControllerEntityPlayer.lib
        RayCast.lib

        DEPS
        spdlog.lib
)