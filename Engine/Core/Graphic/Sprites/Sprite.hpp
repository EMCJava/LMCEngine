//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <memory>

class Sprite : public ConceptRenderable
    , public Orientation
{
    DECLARE_CONCEPT( Sprite, ConceptRenderable )

public:
    Sprite( ) = default;

    void
    Render( ) override;

    void
    SetShader( const std::shared_ptr<Shader>& shader );

    Shader*
    GetShader( );

    /*
     *
     * Setup/Recreate sprite
     * Call when sprite changes, e.g. geometry changes, size change, window resize
     *
     * */
    virtual void
    SetupSprite( ) = 0;

protected:
    std::shared_ptr<Shader> m_Shader;
    uint32_t                m_VAO { }, m_VBO { }, m_EBO { };

    ENABLE_IMGUI( Sprite )
};