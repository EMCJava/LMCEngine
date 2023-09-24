//
// Created by samsa on 9/10/2023.
//

#include "Text.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Graphic/API/OpenGL.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/UI/Font/Font.hpp>
#include <utility>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS( Text )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( Text, Sprite, m_Text, m_Scale, m_Color, m_TextCoordinate, m_Font )

Text::Text( const std::string& Text )
{
    m_Text = std::move( Text );

    m_Shader = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultFontShader" );
}

void
Text::Render( )
{
    if ( !m_Enabled ) return;

    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    // activate corresponding render state
    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrixNonOffset( ) );
        }

        m_Shader->SetVec3( "textColor", m_Color );
    }

    gl->ActiveTexture( GL_TEXTURE0 );
    gl->BindVertexArray( m_VAO );

    auto TextXCoordinate = m_TextCoordinate.x;

    // iterate through all characters
    for ( const char& Char : m_Text )
    {
        const auto ch = m_Font->GetCharacter( Char );

        REQUIRED_IF( ch != nullptr )
        {
            float xpos = TextXCoordinate + ch->Bearing.first * m_Scale;
            float ypos = m_TextCoordinate.y - ( ch->Size.second - ch->Bearing.second ) * m_Scale;

            float w = ch->Size.first * m_Scale;
            float h = ch->Size.second * m_Scale;
            // update VBO for each character
            float vertices[ 6 ][ 4 ] = {
                {    xpos, ypos + h, 0.0f, 0.0f},
                {    xpos,     ypos, 0.0f, 1.0f},
                {xpos + w,     ypos, 1.0f, 1.0f},

                {    xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w,     ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}
            };
            // render glyph texture over quad
            gl->BindTexture( GL_TEXTURE_2D, ch->TextureID );
            // update content of VBO memory
            gl->BindBuffer( GL_ARRAY_BUFFER, m_VBO );
            gl->BufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
            gl->BindBuffer( GL_ARRAY_BUFFER, 0 );
            // render quad
            gl->DrawArrays( GL_TRIANGLES, 0, 6 );
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            TextXCoordinate += ( ch->Advance >> 6 ) * m_Scale;   // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }

    gl->BindVertexArray( 0 );
    gl->BindTexture( GL_TEXTURE_2D, 0 );
}

void
Text::SetupSprite( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GL_CHECK( gl->GenVertexArrays( 1, &m_VAO ) );
    GL_CHECK( gl->GenBuffers( 1, &m_VBO ) );
    GL_CHECK( gl->BindVertexArray( m_VAO ) );

    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_VBO ) );
    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW ) );

    GL_CHECK( gl->EnableVertexAttribArray( 0 ) );
    GL_CHECK( gl->VertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), nullptr ) );

    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, 0 ) );
    GL_CHECK( gl->BindVertexArray( 0 ) );
}

uint32_t
Text::GetTextPixelWidth( )
{
    uint32_t TotalWidth = 0;

    for ( const char& Char : m_Text )
    {
        const auto ch = m_Font->GetCharacter( Char );
        REQUIRED_IF( ch != nullptr )
        {
            TotalWidth += ( ch->Advance >> 6 ) * m_Scale;   // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }

    return TotalWidth;
}

uint32_t
Text::GetTextPixelHeight( )
{
    uint32_t MaxHeight = 0;

    for ( const char& Char : m_Text )
    {
        const auto ch = m_Font->GetCharacter( Char );
        REQUIRED_IF( ch != nullptr )
        {
            MaxHeight = std::max( static_cast<uint32_t>( ch->Height * m_Scale ), MaxHeight );   // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }

    return MaxHeight;
}
