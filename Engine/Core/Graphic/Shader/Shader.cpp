//
// Created by loyus on 7/16/2023.
//

#include "Shader.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>

DEFINE_CONCEPT_DS( Shader )
DEFINE_SIMPLE_IMGUI_TYPE( Shader, m_ShaderProgram )

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
    gl->UniformMatrix4fv( GetUniformLocation( Name ), 1, GL_FALSE, glm::value_ptr( mat ) );
}

void
Shader::SetFloat( const std::string& Name, float Value ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->Uniform1f( GetUniformLocation( Name ), Value );
}

void
Shader::SetProgram( std::shared_ptr<ShaderProgram>& SProgram )
{
    m_ShaderProgram = SProgram;
}

void
Shader::SetUniform( int UniformLocation, float Value ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->Uniform1f( UniformLocation, Value );
}

void
Shader::SetUniform( int UniformLocation, const glm::mat4& mat ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->UniformMatrix4fv( UniformLocation, 1, GL_FALSE, glm::value_ptr( mat ) );
}

void
Shader::SetUniform( int UniformLocation, const glm::vec4& vec ) const
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->Uniform4fv( UniformLocation, 1, glm::value_ptr( vec ) );
}
