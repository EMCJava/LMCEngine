//
// Created by loyus on 7/16/2023.
//

#include "Sprite.hpp"

#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT( Sprite, ConceptRenderable )
DEFINE_SIMPLE_IMGUI_TYPE( Sprite, m_VAO, m_VBO, m_EBO )

Sprite::~Sprite( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GL_CHECK( gl->DeleteVertexArrays( 1, &m_VAO ) );
    GL_CHECK( gl->DeleteBuffers( 1, &m_VBO ) );
    GL_CHECK( gl->DeleteBuffers( 1, &m_EBO ) );

    m_VAO = m_VBO = m_EBO = 0;
}

void
Sprite::Render( )
{
    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrix( ) );
        }

        m_Shader->SetMat4( "modelMatrix", GetTranslationMatrix( ) * GetRotationMatrix( ) );
    }
}

void
Sprite::SetShader( const std::shared_ptr<Shader>& shader )
{
    m_Shader = shader;
}

Shader*
Sprite::GetShader( )
{
    return m_Shader.get( );
}
