//
// Created by loyus on 7/13/2023.
//

#pragma once

#include "ConceptList.hpp"

#include <glm/glm.hpp>

#include <variant>

class ConceptRenderable : public ConceptList
{
    DECLARE_CONCEPT( ConceptRenderable, ConceptList )

public:
    using ShaderUniformTypes = std::variant<glm::mat4, glm::vec4, glm::vec3, float>;

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
    SetShaderUniform( const std::string& UniformName, const ShaderUniformTypes& Value );

    // All user defined uniforms
    void
    ApplyShaderUniforms( );

    void
    ClearShaderUniforms( );

    void CopyShaderUniforms( const ConceptRenderable& other );

protected:
    class PureConceptCamera*      m_ActiveCamera = nullptr;
    std::shared_ptr<class Shader> m_Shader;

    std::map<int, ShaderUniformTypes> m_ShaderUniformSaves;
    std::map<std::string, int>        m_ShaderUniformIDs;

    ENABLE_IMGUI( ConceptRenderable )
};
