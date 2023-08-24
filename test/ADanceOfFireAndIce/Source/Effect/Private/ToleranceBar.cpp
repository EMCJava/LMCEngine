//
// Created by loyus on 8/21/2023.
//

#include "ToleranceBar.hpp"
#include "ToleranceBarStrike.hpp"

#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

DEFINE_CONCEPT_DS( ToleranceBar )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( ToleranceBar, SpriteSquareTexture, m_BarImage, m_BarShader )

const char* vertexAlphaTextureShaderSource   = "#version 330 core\n"
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
const char* fragmentAlphaTextureShaderSource = "#version 330 core\n"
                                               "out vec4 FragColor;\n"
                                               "in vec2 TexCoord;\n"
                                               "uniform sampler2D sample_texture;\n"
                                               "uniform float alpha;\n"
                                               "void main()\n"
                                               "{\n"
                                               "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                               "   FragColor = texColor * vec4(1, 1, 1, alpha);\n"
                                               "}\n\0";

ToleranceBar::ToleranceBar( int Width, int Height )
    : SpriteSquareTexture( Width, Height )
{ }

void
ToleranceBar::Render( )
{
    BindTexture( );

    if ( m_Shader != nullptr )
    {
        m_Shader->Bind( );
        if ( m_ActiveCamera != nullptr )
        {
            m_Shader->SetMat4( "projectionMatrix", m_ActiveCamera->GetProjectionMatrixNonOffset( ) );
        }

        const auto CameraSize = m_ActiveCamera->GetDimensions( );
        if ( CameraSize != m_PreviousCameraSize ) [[unlikely]]
        {
            SetCoordinate( 0, -CameraSize.second * 0.45F / m_ActiveCamera->GetScale( ) );
            m_PreviousCameraSize = CameraSize;
        }

        m_Shader->SetMat4( "modelMatrix", GetTranslationMatrix( ) * GetRotationMatrix( ) );
        SpriteSquare::PureRender( );
    }

    GetConcepts( m_ToleranceBarStrikeCache );
    m_ToleranceBarStrikeCache.ForEach( []( auto& ToleranceBarStrike ) {
        ToleranceBarStrike->Render( );
    } );
}

void
ToleranceBar::AddBar( FloatTy Location )
{
    auto Strike = AddConcept<ToleranceBarStrike>( m_BarSize.first, m_BarSize.second );
    ( *Strike ) << *m_ToleranceBarStrikeTemplate;
    Strike->SetCoordinate( Location * m_Width / 2, -m_PreviousCameraSize.second * 0.45F / m_ActiveCamera->GetScale( ) );
}

void
ToleranceBar::SetupBarTemplate( )
{
    m_BarImage = std::make_shared<PureConceptImage>( );
    if ( !m_BarImage->LoadImage( "Assets/Texture/bar.png" ) )
    {
        throw std::runtime_error( "Failed to load image: Assets/Texture/bar.png" );
    }

    auto SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( vertexAlphaTextureShaderSource, fragmentAlphaTextureShaderSource );
    auto BarShader = std::make_shared<Shader>( );
    BarShader->SetProgram( SProgram );

    m_BarSize                    = m_BarImage->GetImageDimension( );
    m_ToleranceBarStrikeTemplate = std::make_unique<ToleranceBarStrike>( m_BarSize.first, m_BarSize.second );

    m_ToleranceBarStrikeTemplate->SetOrigin( m_BarSize.first / 2.F, m_BarSize.second / 2.F );
    m_ToleranceBarStrikeTemplate->SetShader( BarShader );
    m_ToleranceBarStrikeTemplate->SetImage( m_BarImage );
    m_ToleranceBarStrikeTemplate->SetActiveCamera( m_ActiveCamera );

    m_ToleranceBarStrikeTemplate->SetLifeTime( 5 );

    m_ToleranceBarStrikeTemplate->SetupSprite( );
}

void
ToleranceBar::SetupSprite( )
{
    SpriteSquareTexture::SetupSprite( );
    SetupBarTemplate( );
}
