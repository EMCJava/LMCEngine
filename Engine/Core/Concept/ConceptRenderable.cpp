//
// Created by loyus on 7/13/2023.
//

#include "ConceptRenderable.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCameraStack.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

DEFINE_CONCEPT_DS( ConceptRenderable )
DEFINE_SIMPLE_IMGUI_TYPE( ConceptRenderable, m_ActiveCamera, m_Shader, m_ShaderUniformSaves )

void
ConceptRenderable::SetActiveCamera( class PureConceptCamera* ActiveCamera )
{
    m_ActiveCamera = ActiveCamera;
}

ConceptRenderable::ConceptRenderable( )
{
    SetActiveCamera( Engine::GetEngine( )->GetGlobalResourcePool( )->Get<PureConceptCameraStack>( "DefaultCameraStack" )->GetCamera( ).get( ) );
}

void
ConceptRenderable::SetShader( const std::shared_ptr<Shader>& shader )
{
    ClearShaderUniforms( );
    m_Shader = shader;
}

Shader*
ConceptRenderable::GetShader( )
{
    return m_Shader.get( );
}

void
ConceptRenderable::SetCameraMatrix( )
{
    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
        }
    }
}

void
ConceptRenderable::SetShaderUniform( const std::string& UniformName, const ConceptRenderable::ShaderUniformTypes& Value )
{
    REQUIRED_IF( m_Shader != nullptr )
    {
        m_ShaderUniformSaves[ m_ShaderUniformIDs[ UniformName ] = m_Shader->GetUniformLocation( UniformName ) ] = Value;
    }
}

void
ConceptRenderable::ApplyShaderUniforms( )
{
    for ( const auto& ShaderUniform : m_ShaderUniformSaves )
    {
        if ( ShaderUniform.first == -1 ) continue;
        std::visit( [ this, Location = ShaderUniform.first ]( const auto& Value ) { m_Shader->SetUniform( Location, Value ); }, ShaderUniform.second );
    }
}

void
ConceptRenderable::ClearShaderUniforms( )
{
    m_ShaderUniformSaves.clear( );
}

void
ConceptRenderable::CopyShaderUniforms( const ConceptRenderable& other )
{
    m_ShaderUniformIDs = other.m_ShaderUniformIDs;

    for ( auto& [ UniformName, OtherID ] : m_ShaderUniformIDs )
    {
        m_ShaderUniformSaves[ m_ShaderUniformIDs[ UniformName ] = m_Shader->GetUniformLocation( UniformName ) ] = other.m_ShaderUniformSaves.at( OtherID );
    }
}
