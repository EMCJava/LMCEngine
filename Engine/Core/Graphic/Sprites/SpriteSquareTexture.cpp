//
// Created by loyus on 23/7/2023.
//

#include "SpriteSquareTexture.hpp"

#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

DEFINE_CONCEPT_DS( SpriteSquareTexture )
DEFINE_NECESSARY_IMGUI_TYPE( SpriteSquareTexture )

void
SpriteSquareTexture::ToImGuiWidgetInternal( const char* Name, SpriteSquareTexture* Value, bool ShouldAddSeparator )
{
    if ( ShouldAddSeparator )
    {
        ImGui::SeparatorText( "SpriteSquareTexture" );
    } else
    {
        ImGuiGroup::BeginGroupPanel( Name, ImVec2 { -1, 0 } );
    }

    ToImGuiPointerSwitch( "m_TextureID", &Value->m_TextureID );
    // Texture preview
    ImGui::SameLine( );
    ImGui::Text( "(Preview)" );
    if ( ImGui::BeginItemTooltip( ) )
    {
        ImGui::PushTextWrapPos( ImGui::GetFontSize( ) * 35.0f );
        ImGui::Image( reinterpret_cast<void*>( Value->m_TextureID ),
                      ImVec2 { 50, 50 }, ImVec2( 0, 1 ), ImVec2( 1, 0 ),
                      ImVec4 { 1, 1, 1, 1 },
                      ImGui::GetStyleColorVec4( ImGuiCol_Text ) );

        ImGui::PopTextWrapPos( );
        ImGui::EndTooltip( );
    }

    SIMPLE_LIST_DEFAULT_IMGUI_TYPE( m_TexturePath );
    SpriteSquare::ToImGuiWidgetInternal( Name, Value, ShouldAddSeparator );
    if ( !ShouldAddSeparator )
    {
        ImGui::Spacing( );
        ImGuiGroup::EndGroupPanel( );
    }
}

void
SpriteSquareTexture::SetupSprite( )
{
    spdlog::info( "Setting up texture {}", m_TexturePath );

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    float vertices[] = {
        // positions   // texture coords
        1.f, 1.f, 0.f, 1.0f, 1.0f,   // top right
        1.f, 0.f, 0.f, 1.0f, 0.0f,   // bottom right
        0.f, 0.f, 0.f, 0.0f, 0.0f,   // bottom left
        0.f, 1.f, 0.f, 0.0f, 1.0f    // top left
    };

    for ( int i = 0; i < std::size( vertices ); i += 5 )
    {
        vertices[ i + 0 ] *= m_Width;
        vertices[ i + 1 ] *= m_Height;
    }

    unsigned int indices[] = {
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
    GL_CHECK( gl->VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), nullptr ) )
    GL_CHECK( gl->EnableVertexAttribArray( 0 ) )

    GL_CHECK( gl->VertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void*) ( 3 * sizeof( float ) ) ) )
    GL_CHECK( gl->EnableVertexAttribArray( 1 ) )

    GL_CHECK( gl->GenBuffers( 1, &m_EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW ) )

    LoadTexture( );
}

void
SpriteSquareTexture::SetTexturePath( const std::string& TexturePath )
{
    m_TexturePath = TexturePath;
}

void
SpriteSquareTexture::Render( )
{
    BindTexture( );
    SpriteSquare::Render( );
}

uint32_t
SpriteSquareTexture::GetTextureID( ) const noexcept
{
    return m_TextureID;
}

void
SpriteSquareTexture::BindTexture( )
{
    if ( m_Shader != nullptr )
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        gl->ActiveTexture( GL_TEXTURE0 );
        gl->BindTexture( GL_TEXTURE_2D, m_TextureID );
    }
}

void
SpriteSquareTexture::LoadTexture( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    gl->GenTextures( 1, &m_TextureID );
    gl->BindTexture( GL_TEXTURE_2D, m_TextureID );
    // set the texture wrapping parameters
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );   // set texture wrapping to GL_REPEAT (default wrapping method)
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // set texture filtering parameters
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    if ( m_TextureImage == nullptr )
    {
        m_TextureImage = std::make_shared<PureConceptImage>( m_TexturePath );
    }

    const bool HasCorrectData = ( m_TexturePath.empty( ) || m_TextureImage->GetImagePath( ) == m_TexturePath ) && m_TextureImage->GetImageData( );
    if ( HasCorrectData || m_TextureImage->LoadImage( m_TexturePath ) )
    {
        GLenum Format;
        switch ( m_TextureImage->GetImageChannelCount( ) )
        {
        case 1: Format = GL_RED; break;
        case 2: Format = GL_RGB; break;
        case 4: Format = GL_RGBA; break;

        default:
            throw std::runtime_error( "Unsupported image channel count" );
        }

        const auto ImageDimension = m_TextureImage->GetImageDimension( );
        gl->TexImage2D( GL_TEXTURE_2D, 0, Format, ImageDimension.first, ImageDimension.second, 0, Format, GL_UNSIGNED_BYTE, m_TextureImage->GetImageData( ) );
        gl->GenerateMipmap( GL_TEXTURE_2D );
    } else
    {
        throw std::runtime_error( ( "Failed to load image: " + m_TexturePath ).c_str( ) );
    }

    m_Shader->Bind( );
    gl->Uniform1i( m_Shader->GetUniformLocation( "sample_texture" ), 0 );
}

void
SpriteSquareTexture::SetImage( std::shared_ptr<struct PureConceptImage> Image )
{
    m_TextureImage = Image;
}
