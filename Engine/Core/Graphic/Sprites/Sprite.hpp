//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <map>
#include <string>
#include <variant>
#include <memory>

class Sprite : public ConceptRenderable
    , public Orientation
{
    DECLARE_CONCEPT( Sprite, ConceptRenderable )

public:

    void
    Render( ) override;

    /*
     *
     * Setup/Recreate sprite
     * Call when sprite changes, e.g. geometry changes, size change, window resize
     *
     * */
    virtual void
    SetupSprite( ) = 0;

    [[nodiscard]] uint32_t
    GetVAO( ) const { return m_VAO; }
    [[nodiscard]] uint32_t
    GetVBO( ) const { return m_VBO; }
    [[nodiscard]] uint32_t
    GetEBO( ) const { return m_EBO; }

protected:
    uint32_t                m_VAO { }, m_VBO { }, m_EBO { };
    /*
     *
     * Useful when multiple concept uses the same context
     *
     * */
    bool m_ShouldDeallocateGLResources { true };

    ENABLE_IMGUI( Sprite )
};