//
// Created by samsa on 7/8/2023.
//

#include "ToleranceVisualizer.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/HotReloadFrameBuffer/HotReloadFrameBuffer.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <imgui.h>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS( ToleranceVisualizer )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ToleranceVisualizer, SpriteSquareTexture, m_FadeShader, m_BarShader, m_PassFrameBuffer )

ToleranceVisualizer::ToleranceVisualizer( int Width, int Height, const std::string& BarTexturePath )
    : SpriteSquareTexture( Width, Height )
{
    m_PassFrameBuffer = std::make_unique<HotReloadFrameBuffer>( );
    m_PassFrameBuffer->SetGLContext( Engine::GetEngine( )->GetGLContext( ) );
    m_PassFrameBuffer->CreateFrameBuffer( Width, Height );

    {
        m_PassFrameBuffer->BindFrameBuffer( );
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        gl->ClearColor( 0, 0, 0, 0 );
        gl->Clear( GL_COLOR_BUFFER_BIT );
        m_PassFrameBuffer->UnBindFrameBuffer( );
    }

    SetupFadeShader( );
    SetupBarShader( BarTexturePath );
}

void
ToleranceVisualizer::Render( )
{
    const auto BarTextureID = m_TextureID;

    // First fade the last frame
    m_PassFrameBuffer->BindFrameBuffer( );
    m_TextureID = m_PassFrameBuffer->GetTextureID( );
    m_Shader    = m_FadeShader;
    SpriteSquareTexture::Render( );
    m_PassFrameBuffer->UnBindFrameBuffer( );

    // Render the final result
    if ( m_BarShader != nullptr )
    {
        m_BarShader->Bind( );

        const auto* gl = Engine::GetEngine( )->GetGLContext( );

        gl->ActiveTexture( GL_TEXTURE0 );
        gl->Uniform1i( m_BarShader->GetUniformLocation( "bar_texture" ), 0 );
        gl->BindTexture( GL_TEXTURE_2D, BarTextureID );

        gl->ActiveTexture( GL_TEXTURE1 );
        gl->Uniform1i( m_BarShader->GetUniformLocation( "visualizer_texture" ), 1 );
        gl->BindTexture( GL_TEXTURE_2D, m_PassFrameBuffer->GetTextureID( ) );

        if ( m_ActiveCamera != nullptr )
        {
            m_BarShader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrixNonOffset( ) );
        }

        GL_CHECK( gl->BindVertexArray( m_VAO ) )
        GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
        GL_CHECK( gl->DrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr ) )
    }

    m_TextureID = BarTextureID;
}

void
ToleranceVisualizer::SetupFadeShader( )
{

    const char* vertexFadeTextureShaderSource = "#version 330 core\n"
                                                "layout (location = 0) in vec3 aPos;\n"
                                                "layout (location = 1) in vec2 aTexCoord;\n"
                                                "out vec2 TexCoord;\n"
                                                "uniform mat4 projectionMatrix;\n"
                                                "uniform mat4 modelMatrix;\n"
                                                "void main()\n"
                                                "{\n"
                                                "   gl_Position = projectionMatrix * modelMatrix * vec4(aPos, 1.0);\n"
                                                "   TexCoord = aTexCoord;\n"
                                                "}\0";

    const char* fragmentFadeTextureShaderSource = "#version 330 core\n"
                                                  "out vec4 FragColor;\n"
                                                  "in vec2 TexCoord;\n"
                                                  "uniform sampler2D sample_texture;\n"
                                                  "void main()\n"
                                                  "{\n"
                                                  "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                                  "   FragColor = mix(texColor, vec4(0), 0.99);\n"
                                                  "}\n\0";

    auto SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( vertexFadeTextureShaderSource, fragmentFadeTextureShaderSource );
    m_FadeShader = std::make_shared<Shader>( );
    m_FadeShader->SetProgram( SProgram );
}

void
ToleranceVisualizer::SetupBarShader( const std::string& BarTexturePath )
{

    const char* vertexBarShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "layout (location = 1) in vec2 aTexCoord;\n"
                                        "out vec2 TexCoord;\n"
                                        "uniform mat4 projectionMatrix;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   gl_Position = projectionMatrix * vec4(aPos, 1.0);\n"
                                        "   TexCoord = aTexCoord;\n"
                                        "}\0";

    const char* fragmentBarShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 TexCoord;\n"
                                          "uniform sampler2D bar_texture;\n"
                                          "uniform sampler2D visualizer_texture;\n"
                                          "void main()\n"
                                          "{\n"
                                          "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                          "   FragColor = mix(texColor, vec4(0), 0.99);\n"
                                          "}\n\0";

    auto SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( vertexBarShaderSource, fragmentBarShaderSource );
    m_BarShader = std::make_shared<Shader>( );
    m_BarShader->SetProgram( SProgram );

    SetTexturePath( BarTexturePath );
    SpriteSquareTexture::SetupSprite( );
}
