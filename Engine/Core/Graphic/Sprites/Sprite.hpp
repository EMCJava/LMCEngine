//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <glm/glm.hpp>

#include <map>
#include <string>
#include <variant>
#include <memory>

class Sprite : public ConceptRenderable
    , public Orientation
{
    DECLARE_CONCEPT( Sprite, ConceptRenderable )

    void
    ApplyShaderUniforms( );

public:
    using ShaderUniformTypes = std::variant<glm::mat4, glm::vec4, float>;

    Sprite( ) = default;

    void
    Render( ) override;

    void
    SetShader( const std::shared_ptr<Shader>& shader );

    Shader*
    GetShader( );

    void
    SetShaderMatrix( );

    /*
     *
     * Setup/Recreate sprite
     * Call when sprite changes, e.g. geometry changes, size change, window resize
     *
     * */
    virtual void
    SetupSprite( ) = 0;

    void
    SetAbsolutePosition( bool Absolute = true ) { m_IsAbsolutePosition = Absolute; }

    void
    SetShaderUniform( std::string UniformName, const ShaderUniformTypes& Value );

    void
    ClearShaderUniforms( );

protected:
    std::shared_ptr<Shader> m_Shader;
    uint32_t                m_VAO { }, m_VBO { }, m_EBO { };

    std::map<int, ShaderUniformTypes> m_ShaderUniformSaves;

    /*
     *
     * Useful when multiple concept uses the same context
     *
     * */
    bool m_ShouldDeallocateGLResources { true };

    /*
     *
     * Use screen coordinate instead of world coordinates
     *
     * */
    bool m_IsAbsolutePosition { false };

    ENABLE_IMGUI( Sprite )
};