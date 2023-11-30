//
// Created by samsa on 11/30/2023.
//

#include "RenderableMeshHitBox.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

DEFINE_CONCEPT_DS( RenderableMeshHitBox )

void
RenderableMeshHitBox::RenderFrame( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    gl->PolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    GL_CHECK( gl->BindVertexArray( m_BufferHandle.VAO ) )
    GL_CHECK( gl->DrawElements( GL_TRIANGLES, m_NumOfIndices, GL_UNSIGNED_INT, nullptr ) )
    gl->PolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void
RenderableMeshHitBox::Render( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    m_Shader->Bind( );
    SetCameraMatrix( );
    ApplyShaderUniforms( );

    if ( gl->IsEnabled( GL_DEPTH_TEST ) )
    {
        GL_CHECK( gl->Disable( GL_DEPTH_TEST ) )
        RenderFrame( );
        GL_CHECK( gl->Enable( GL_DEPTH_TEST ) )
    } else
    {
        RenderFrame( );
    }
}

RenderableMeshHitBox::RenderableMeshHitBox( std::vector<float> Vertices, std::vector<uint32_t> Indices )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GL_CHECK( gl->GenVertexArrays( 1, &m_BufferHandle.VAO ) )
    GL_CHECK( gl->BindVertexArray( m_BufferHandle.VAO ) )

    // 2. copy our vertices array in a buffer for OpenGL to use
    GL_CHECK( gl->GenBuffers( 1, &m_BufferHandle.VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_BufferHandle.VBO ) )
    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( float ) * Vertices.size( ), Vertices.data( ), GL_STATIC_DRAW ) )

    // 3. then set our vertex attributes pointers
    GL_CHECK( gl->VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr ) )
    GL_CHECK( gl->EnableVertexAttribArray( 0 ) )

    GL_CHECK( gl->GenBuffers( 1, &m_BufferHandle.EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_BufferHandle.EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint32_t ) * Indices.size( ), Indices.data( ), GL_STATIC_DRAW ) )
    m_NumOfIndices = Indices.size( );

    m_Shader = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorShader" );
    SetShaderUniform( "fragColor", glm::vec4( 0, 1, 0, 1 ) );
}
