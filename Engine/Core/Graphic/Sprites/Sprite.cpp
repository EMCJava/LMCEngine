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
DEFINE_SIMPLE_IMGUI_TYPE( Sprite, m_VAO, m_VBO, m_EBO, SetupSprite )

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

    SetCameraMatrix( );
    m_Shader->SetMat4( "modelMatrix", GetModelMatrix( ) );
    ApplyShaderUniforms( );
}