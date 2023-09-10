//
// Created by loyus on 7/16/2023.
//

#pragma once

#include "ShaderProgram.hpp"

#include <Engine/Core/Concept/PureConcept.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>

class Shader : public PureConcept
{
    DECLARE_CONCEPT( Shader, PureConcept )
public:
    Shader( ) = default;

    void
    Bind( ) const;

    /*
     *
     * Shader operations
     *
     * */
    int
    GetUniformLocation( const std::string& Name ) const;

    void SetVec3( const std::string& Name, const glm::vec3& mat ) const;

    void SetVec4( const std::string& Name, const glm::vec4& mat ) const;

    void SetMat4( const std::string& Name, const glm::mat4& mat ) const;

    void SetFloat( const std::string& Name, float Value ) const;

    void SetProgram( std::shared_ptr<ShaderProgram>& SProgram );

    auto& GetProgram( ) { return m_ShaderProgram; }

    /*
     *
     * For general type matching
     *
     * */
    void
    SetUniform( int UniformLocation, float Value ) const;

    void
    SetUniform( int UniformLocation, const glm::mat4& mat ) const;

    void
    SetUniform( int UniformLocation, const glm::vec4& vec ) const;

    void
    SetUniform( int UniformLocation, const glm::vec3& vec ) const;

private:
    std::shared_ptr<ShaderProgram> m_ShaderProgram;

    ENABLE_IMGUI( Shader )
};
