//
// Created by loyus on 7/12/2023.
//

#include "SpriteSquare.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS( SpriteSquare )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( SpriteSquare, Sprite, m_Width, m_Height )

SpriteSquare::SpriteSquare( int Width, int Height )
    : m_Width( Width )
    , m_Height( Height )
{
    // SetupSprite();
}

void
SpriteSquare::Render( )
{
    Sprite::Render( );

    PureRender( );
}

void
SpriteSquare::PureRender( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( gl->BindVertexArray( m_VAO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
    GL_CHECK( gl->DrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr ) )
}

void
SpriteSquare::SetupSprite( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    float vertices[] = {
        1.f, 1.f, 0.f,   // top right
        1.f, 0.f, 0.f,   // bottom right
        0.f, 0.f, 0.f,   // bottom left
        0.f, 1.f, 0.f    // top left
    };

    for ( int i = 0; i < std::size( vertices ); i += 3 )
    {
        vertices[ i + 0 ] *= m_Width;
        vertices[ i + 1 ] *= m_Height;
    }

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    GL_CHECK( gl->GenVertexArrays( 1, &m_VAO ) )
    GL_CHECK( gl->BindVertexArray( m_VAO ) )

    // 2. copy our vertices array in a buffer for OpenGL to use
    GL_CHECK( gl->GenBuffers( 1, &m_VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_VBO ) )
    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW ) )

    // 3. then set our vertex attributes pointers
    GL_CHECK( gl->VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr ) )
    GL_CHECK( gl->EnableVertexAttribArray( 0 ) )

    GL_CHECK( gl->GenBuffers( 1, &m_EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW ) )
}
