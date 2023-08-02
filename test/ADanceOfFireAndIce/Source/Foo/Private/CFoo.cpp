//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Scene/Orientation/OrientationCoordinate.hpp>
#include <Engine/Engine.hpp>

#include <Engine/Core/Audio/AudioEngine.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquare.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Input/UserInput.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_MA_SE( CFoo, Concept )

const char* vertexTextureShaderSource   = "#version 330 core\n"
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
const char* fragmentTextureShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 TexCoord;\n"
                                          "uniform sampler2D sample_texture;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec4 texColor = texture(sample_texture, TexCoord);"
                                          "   if(texColor.a < 0.1)"
                                          "        discard;"
                                          "   FragColor = texColor;\n"
                                          "}\n\0";

CFoo::CFoo( )
{
    spdlog::info( "CFoo concept constructor called" );

    auto* MainCamera = AddConcept<PureConceptCamera>( );
    MainCamera->SetCoordinate( -500, 700 );
    MainCamera->SetScale( 1 / 2.f );
    MainCamera->UpdateProjectionMatrix( );

    auto SProgram = std::make_shared<ShaderProgram>( );
    SProgram->Load( vertexTextureShaderSource, fragmentTextureShaderSource );

    auto S1 = std::make_shared<Shader>( );
    S1->SetProgram( SProgram );

    m_TileSpriteSet = AddConcept<TileSpriteSet>( );

    const auto AddDegreeTile = [ & ]( uint32_t Degree ) {
        auto* Sp = m_TileSpriteSet->RegisterSprite( Degree, std::make_unique<SpriteSquareTexture>( 512, 512 ) );

        Sp->SetRotationCenter( 512 / 2, 512 / 2 );
        Sp->SetShader( S1 );
        Sp->SetTexturePath( "Access/Texture/Tile/" + std::to_string( Degree ) + ".png" );
        Sp->SetupSprite( );
    };

    AddDegreeTile( 180 );
    AddDegreeTile( 120 );
    AddDegreeTile( 90 );
    AddDegreeTile( 60 );


    m_TileSpriteSet->SetActiveCamera( MainCamera );
    m_TileSpriteSet->AddTile( { 90 } );
    m_TileSpriteSet->AddTile( { 120 } );
    m_TileSpriteSet->AddTile( { 180 } );
    m_TileSpriteSet->AddTile( { 180 } );
    m_TileSpriteSet->AddTile( { 90 } );
    m_TileSpriteSet->AddTile( { 180 } );
    m_TileSpriteSet->AddTile( { 180 } );
    m_TileSpriteSet->AddTile( { 60 } );


    auto* DDC             = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Access/Audio/Beats.ogg" );
    m_DelayCheckingHandle = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( DDC, true );

    auto* MAC         = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Access/Audio/Papipupipupipa.ogg" );
    m_MainAudioHandle = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( MAC, true, true );

    spdlog::info( "CFoo concept constructor returned" );
}

CFoo::~CFoo( )
{
    spdlog::info( "CFoo concept destructor called" );
}

void
CFoo::Apply( )
{
    auto* Sp = GetConcept<ConceptRenderable>( );

    if ( m_IsCheckingDeviceDelay )
    {
        constexpr int64_t AudioDefaultOffset = 3000;
        const int64_t     PlayPosition       = m_DelayCheckingHandle.GetCurrentAudioOffset( );
        if ( Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( ).isPressed )
        {
            m_UserDeviceOffsetMS = PlayPosition - AudioDefaultOffset;
            spdlog::info( "Offset: {}", m_UserDeviceOffsetMS );
        }

        // End of offset wizard
        if ( PlayPosition == 0 && m_DelayCheckingHandle.IsAudioEnded( ) )
        {
            spdlog::info( "End of offset wizard, final offset: {}ms", m_UserDeviceOffsetMS );
            m_IsCheckingDeviceDelay = false;

            m_MainAudioHandle.Resume( );
        }
    }
}
