//
// Created by loyus on 7/13/2023.
//

#pragma once

#include "Concept.hpp"

#include <glm/glm.hpp>

#include <variant>

class ConceptRenderable : public Concept
{
    DECLARE_CONCEPT( ConceptRenderable, Concept )

public:
    using ShaderUniformTypes = std::variant<glm::mat4, glm::vec4, float>;

    /*
     *
     * Try to set camera to default camera
     *
     * */
    ConceptRenderable( );

    virtual void
    Render( ) = 0;

    void
    SetActiveCamera( class PureConceptCamera* ActiveCamera );

    class PureConceptCamera*
    GetActiveCamera( ) { return m_ActiveCamera; }

    void
    SetShader( const std::shared_ptr<class Shader>& shader );

    class Shader*
    GetShader( );

    void
    SetCameraMatrix( );

    void
    SetShaderUniform( std::string UniformName, const ShaderUniformTypes& Value );

    // All user defined uniforms
    void
    ApplyShaderUniforms( );

    void
    ClearShaderUniforms( );

protected:
    class PureConceptCamera*      m_ActiveCamera = nullptr;
    std::shared_ptr<class Shader> m_Shader;

    std::map<int, ShaderUniformTypes> m_ShaderUniformSaves;

    /*
     *
     * Use screen coordinate instead of world coordinates
     *
     * */
    bool m_IsAbsolutePosition { false };

    ENABLE_IMGUI( ConceptRenderable )
};
