//
// Created by loyus on 7/16/2023.
//

#include "Sprite.hpp"

#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

#include <variant>

DEFINE_CONCEPT( Sprite )
DEFINE_SIMPLE_IMGUI_TYPE( Sprite, m_VAO, m_VBO, m_EBO, m_Shader, m_IsAbsolutePosition )

Sprite::~Sprite( )
{
    if ( m_ShouldDeallocateGLResources )
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

        GL_CHECK( gl->DeleteVertexArrays( 1, &m_VAO ) );
        GL_CHECK( gl->DeleteBuffers( 1, &m_VBO ) );
        GL_CHECK( gl->DeleteBuffers( 1, &m_EBO ) );

        m_VAO = m_VBO = m_EBO = 0;
    }
}

void
Sprite::Render( )
{
    if ( !m_Enabled ) return;

    SetShaderMatrix( );
    ApplyShaderUniforms( );
}

void
Sprite::SetShader( const std::shared_ptr<Shader>& shader )
{
    ClearShaderUniforms( );
    m_Shader = shader;
}

Shader*
Sprite::GetShader( )
{
    return m_Shader.get( );
}

void
Sprite::SetShaderMatrix( )
{
    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            if ( m_IsAbsolutePosition )
            {
                m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrixNonOffset( ) );
            } else
            {
                m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
            }
        }

        m_Shader->SetMat4( "modelMatrix", GetTranslationMatrix( ) * GetRotationMatrix( ) );
    }
}

void
Sprite::SetShaderUniform( std::string UniformName, const Sprite::ShaderUniformTypes& Value )
{
    REQUIRED_IF( m_Shader != nullptr )
    {
        m_ShaderUniformSaves[ m_Shader->GetUniformLocation( UniformName ) ] = Value;
    }
}

void
Sprite::ApplyShaderUniforms( )
{
    for ( const auto& ShaderUniform : m_ShaderUniformSaves )
    {
        std::visit( [ this, Location = ShaderUniform.first ]( const auto& Value ) { m_Shader->SetUniform( Location, Value ); }, ShaderUniform.second );
    }
}

void
Sprite::ClearShaderUniforms( )
{
    m_ShaderUniformSaves.clear( );
}
