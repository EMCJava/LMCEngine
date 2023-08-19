//
// Created by Lo Yu Sum on 14/8/2023.
//

#include "SpriteSquareAnimatedTexture.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

#include <Engine/Core/Graphic/API/OpenGL.hpp>

#ifdef STB_IMAGE_IMPLEMENTATION
#    undef STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

DEFINE_CONCEPT_DS( SpriteSquareAnimatedTexture )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( SpriteSquareAnimatedTexture, SpriteSquareTexture, m_AnimationFrameIndex, m_FrameBoxList, m_Repeat, m_DestroyAfterFinish, m_FrameTime, m_CurrentFrameTimeLeft )

void
SpriteSquareAnimatedTexture::SetTextureGrid( uint32_t HorCount, uint32_t VertCount )
{
    m_HorCount  = HorCount;
    m_VertCount = VertCount;

    const float FrameWidth  = 1.F / HorCount;
    const float FrameHeight = 1.F / VertCount;

    m_FrameBoxList.clear( );
    for ( uint32_t VIndex = 0; VIndex < VertCount; ++VIndex )
    {
        for ( uint32_t HIndex = 0; HIndex < HorCount; ++HIndex )
        {
            AddGrid(
                { HIndex * FrameWidth, 1 - VIndex * FrameHeight },
                { ( HIndex + 1 ) * FrameWidth, 1 - ( VIndex + 1 ) * FrameHeight } );
        }
    }
}

void
SpriteSquareAnimatedTexture::SetAnimationFrameIndex( uint32_t Index )
{
    m_AnimationFrameIndex = Index;
}

size_t
SpriteSquareAnimatedTexture::GetTotalFrame( ) const
{
    return m_FrameBoxList.size( );
}

void
SpriteSquareAnimatedTexture::AddGrid( SpriteSquareAnimatedTexture::FrameBoxListCoordinate TopLeft, SpriteSquareAnimatedTexture::FrameBoxListCoordinate BottomRight )
{
    m_FrameBoxList.emplace_back( TopLeft, BottomRight );
}

void
SpriteSquareAnimatedTexture::Render( )
{
    const auto* gl = Engine::GetEngine( )->GetGLContext( );

    if ( !m_Repeat && m_AnimationFrameIndex >= m_FrameBoxList.size( ) - 1 )
    {
        Destroy( );
        return;
    }

    if ( m_FrameTime != 0 )
    {
        m_CurrentFrameTimeLeft -= Engine::GetEngine( )->GetDeltaSecond( );

        if ( m_CurrentFrameTimeLeft <= 0 )
        {
            NextFrame( );

            m_CurrentFrameTimeLeft = m_FrameTime;
        }
    }

    if ( m_Shader != nullptr )
    {
        gl->ActiveTexture( GL_TEXTURE0 );
        gl->BindTexture( GL_TEXTURE_2D, m_TextureID );
    }

    Sprite::Render( );

    GL_CHECK( gl->BindVertexArray( m_VAO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
    GL_CHECK( gl->DrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) ( 6 * m_AnimationFrameIndex * sizeof( uint32_t ) ) ) )
}

void
SpriteSquareAnimatedTexture::SetupSprite( )
{
    spdlog::info( "Setting up texture {}", m_TexturePath );

    const auto* gl = Engine::GetEngine( )->GetGLContext( );
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    static constexpr auto PointVerticesOccu = 5;

    float vertices[] = {
        // positions   // texture coords
        1.f, 1.f, 0.f, 1.0f, 1.0f,   // top right
        1.f, 0.f, 0.f, 1.0f, 0.0f,   // bottom right
        0.f, 0.f, 0.f, 0.0f, 0.0f,   // bottom left
        0.f, 1.f, 0.f, 0.0f, 1.0f    // top left
    };

    // Set to correct width and height
    for ( int i = 0; i < std::size( vertices ); i += PointVerticesOccu )
    {
        vertices[ i + 0 ] *= m_Width;
        vertices[ i + 1 ] *= m_Height;
    }

    // Copy square for every frame
    auto FrameVerticesArray = std::make_unique<float[]>( std::size( vertices ) * m_FrameBoxList.size( ) );
    for ( int i = 0; i < m_FrameBoxList.size( ); ++i )
    {
        auto* StartPtr = FrameVerticesArray.get( ) + i * std::size( vertices );
        memcpy( StartPtr, vertices, sizeof( vertices ) );

        StartPtr[ 3 ] = m_FrameBoxList[ i ].second.first;
        StartPtr[ 4 ] = m_FrameBoxList[ i ].first.second;

        StartPtr[ 3 + 5 ] = m_FrameBoxList[ i ].second.first;
        StartPtr[ 4 + 5 ] = m_FrameBoxList[ i ].second.second;

        StartPtr[ 3 + 10 ] = m_FrameBoxList[ i ].first.first;
        StartPtr[ 4 + 10 ] = m_FrameBoxList[ i ].second.second;

        StartPtr[ 3 + 15 ] = m_FrameBoxList[ i ].first.first;
        StartPtr[ 4 + 15 ] = m_FrameBoxList[ i ].first.second;
    }

    uint32_t indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    auto FrameIndicesArray = std::make_unique<uint32_t[]>( std::size( indices ) * m_FrameBoxList.size( ) );
    for ( int i = 0; i < m_FrameBoxList.size( ); ++i )
    {
        auto* StartPtr = FrameIndicesArray.get( ) + i * std::size( indices );
        for ( int j = 0; j < std::size( indices ); ++j )
        {
            StartPtr[ j ] = indices[ j ] + ( std::size( vertices ) / PointVerticesOccu ) * i;
        }
    }

    GL_CHECK( gl->GenVertexArrays( 1, &m_VAO ) )
    GL_CHECK( gl->BindVertexArray( m_VAO ) )

    // 2. copy our vertices array in a buffer for OpenGL to use
    GL_CHECK( gl->GenBuffers( 1, &m_VBO ) )
    GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_VBO ) )
    GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( vertices ) * m_FrameBoxList.size( ), FrameVerticesArray.get( ), GL_STATIC_DRAW ) )

    // 3. then set our vertex attributes pointers
    GL_CHECK( gl->VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, PointVerticesOccu * sizeof( float ), nullptr ) )
    GL_CHECK( gl->EnableVertexAttribArray( 0 ) )

    GL_CHECK( gl->VertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, PointVerticesOccu * sizeof( float ), (void*) ( 3 * sizeof( float ) ) ) )
    GL_CHECK( gl->EnableVertexAttribArray( 1 ) )

    GL_CHECK( gl->GenBuffers( 1, &m_EBO ) )
    GL_CHECK( gl->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO ) )
    GL_CHECK( gl->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ) * m_FrameBoxList.size( ), FrameIndicesArray.get( ), GL_STATIC_DRAW ) )

    // Setup texture

    // texture 1
    // ---------
    gl->GenTextures( 1, &m_TextureID );
    gl->BindTexture( GL_TEXTURE_2D, m_TextureID );
    // set the texture wrapping parameters
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );   // set texture wrapping to GL_REPEAT (default wrapping method)
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // set texture filtering parameters
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load( true );   // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load( m_TexturePath.c_str( ), &width, &height, &nrChannels, 0 );
    if ( data )
    {
        GLenum Format;
        if ( nrChannels == 1 )
        {
            Format = GL_RED;
        } else if ( nrChannels == 3 )
        {
            Format = GL_RGB;
        } else if ( nrChannels == 4 )
        {
            Format = GL_RGBA;
        }

        spdlog::info( "Loaded texture {} with size {} x {} C {}", m_TexturePath, width, height, nrChannels );

        gl->TexImage2D( GL_TEXTURE_2D, 0, Format, width, height, 0, Format, GL_UNSIGNED_BYTE, data );
        gl->GenerateMipmap( GL_TEXTURE_2D );
    } else
    {
        spdlog::error( "Failed to load texture" );
    }
    stbi_image_free( data );

    m_Shader->Bind( );
    gl->Uniform1i( m_Shader->GetUniformLocation( "sample_texture" ), 0 );
}

void
SpriteSquareAnimatedTexture::NextFrame( )
{
    m_AnimationFrameIndex = ( m_AnimationFrameIndex + 1 ) % m_FrameBoxList.size( );
}

void
SpriteSquareAnimatedTexture::SetFrameTime( FloatTy DeltaTime )
{
    m_FrameTime = DeltaTime;
}

void
SpriteSquareAnimatedTexture::SetRepeat( bool Repeat, bool DestroyAfterFinish )
{
    m_Repeat             = Repeat;
    m_DestroyAfterFinish = DestroyAfterFinish;
}