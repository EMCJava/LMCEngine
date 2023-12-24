//
// Created by samsa on 12/22/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include <memory>

struct TextureSlot {
    std::shared_ptr<class Texture> Texture;
    uint8_t                        Slot = 0;
    ENABLE_IMGUI( TextureSlot )
};

struct Material {
    TextureSlot ColorTexture;
    TextureSlot NormalTexture;

    void ActivateMaterial( ) const;
    ENABLE_IMGUI( Material )
};
