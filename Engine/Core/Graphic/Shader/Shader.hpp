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

    void SetMat4( const std::string& Name, const glm::mat4& mat ) const;

    void SetProgram( std::shared_ptr<ShaderProgram>& SProgram );

    auto& GetProgram( ) { return m_ShaderProgram; }

private:
    std::shared_ptr<ShaderProgram> m_ShaderProgram;

    ENABLE_IMGUI( Shader )
};
