//
// Created by samsa on 12/22/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include <memory>

#include <glm/vec4.hpp>

struct TextureSlot {
    std::shared_ptr<class Texture> Texture;
    uint8_t                        Slot = 0;

    void
    LoadTexture( const std::string& TexturePath, int TextureSlot = 0 );

    ENABLE_IMGUI( TextureSlot )
};

struct Material {
    TextureSlot ColorTexture;
    TextureSlot NormalTexture;

    glm::vec4 DiffuseColor = glm::vec4( 1.0f );

    void ActivateMaterial( class Shader* Shader ) const;

    ENABLE_IMGUI( Material )
};
