//
// Created by samsa on 12/21/2023.
//

#include "Texture.hpp"

#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

DEFINE_NECESSARY_IMGUI_TYPE( Texture )

void
Texture::ToImGuiWidgetInternal( const char* Name, Texture* Value, bool ShouldAddSeparator )
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
    if ( !ShouldAddSeparator )
    {
        ImGui::Spacing( );
        ImGuiGroup::EndGroupPanel( );
    }
}


Texture::Texture( std::shared_ptr<PureConceptImage> Img )
{
    SetImage( std::move( Img ) );
}

Texture::Texture( const std::string& TexturePath )
{
    SetTexturePath( TexturePath );
}

void
Texture::SetTexturePath( const std::string& TexturePath )
{
    // Might be inconsistent with m_TextureImage
    m_TexturePath = TexturePath;
}

void
Texture::SetImage( std::shared_ptr<PureConceptImage> Image )
{
    m_TextureImage = std::move( Image );
    if ( m_TextureImage && m_TextureImage->GetImageData( ) )
    {
        m_TexturePath = m_TextureImage->GetImagePath( );
    }
}

void
Texture::LoadTexture( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    GL_CHECK( gl->GenTextures( 1, &m_TextureID ) );
    GL_CHECK( gl->BindTexture( GL_TEXTURE_2D, m_TextureID ) );
    // set the texture wrapping parameters
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );   // set texture wrapping to GL_REPEAT (default wrapping method)
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ) );
    // set texture filtering parameters
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );

    if ( m_TextureImage == nullptr )
    {
        m_TextureImage = std::make_shared<PureConceptImage>( m_TexturePath );
    }

    const bool HasCorrectData = ( m_TexturePath.empty( ) || m_TextureImage->GetImagePath( ) == m_TexturePath ) && m_TextureImage->GetImageData( );
    if ( HasCorrectData || m_TextureImage->LoadImage( m_TexturePath ) )
    {
        GLint Format;
        switch ( m_TextureImage->GetImageChannelCount( ) )
        {
        case 1: Format = GL_RED; break;
        case 2: Format = GL_RGB; break;
        case 4: Format = GL_RGBA; break;

        default:
            throw std::runtime_error( "Unsupported image channel count" );
        }

        const auto ImageDimension = m_TextureImage->GetImageDimension( );
        GL_CHECK( gl->TexImage2D( GL_TEXTURE_2D, 0, Format, ImageDimension.first, ImageDimension.second, 0, Format, GL_UNSIGNED_BYTE, m_TextureImage->GetImageData( ) ) );
        GL_CHECK( gl->GenerateMipmap( GL_TEXTURE_2D ) );
    } else
    {
        throw std::runtime_error( ( "Failed to load image: " + m_TexturePath ).c_str( ) );
    }
}

void
Texture::BindTexture( int BindAt ) const
{
    static_assert( GL_TEXTURE31 - GL_TEXTURE0 == 31 );

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    gl->ActiveTexture( GL_TEXTURE0 + BindAt );
    gl->BindTexture( GL_TEXTURE_2D, m_TextureID );
}

uint32_t
Texture::GetTextureID( ) const noexcept
{
    return m_TextureID;
}
