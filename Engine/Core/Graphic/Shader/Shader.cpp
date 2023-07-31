//
// Created by loyus on 7/16/2023.
//

#include "Shader.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

#include <fstream>

void
Shader::Bind( ) const
{
    m_ShaderProgram->Bind( );
}

int
Shader::GetUniformLocation( const std::string& Name ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    return gl->GetUniformLocation( m_ShaderProgram->GetProgramID( ), Name.c_str( ) );
}

void
Shader::SetMat4( const std::string& Name, const glm::mat4& mat ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->UniformMatrix4fv( GetUniformLocation( Name ), 1, GL_FALSE, &mat[ 0 ][ 0 ] );
}

void
Shader::SetProgram( std::shared_ptr<ShaderProgram>& SProgram )
{
    m_ShaderProgram = SProgram;
}
