//
// Created by loyus on 7/16/2023.
//

#pragma once

#include "ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <string>

class Shader
{
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

private:
    std::shared_ptr<ShaderProgram> m_ShaderProgram;
};
