//
// Created by loyus on 7/16/2023.
//

#include "HotReloadFrameBuffer.hpp"

#include <spdlog/spdlog.h>

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <glad/gl.h>

HotReloadFrameBuffer::~HotReloadFrameBuffer( )
{
    const auto* gl = m_GLContext;
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    if ( m_FBO != 0 )
    {
        m_GLContext->DeleteFramebuffers( 1, &m_FBO );
        m_FBO = 0;
    }

    if ( m_TextureID != 0 )
    {
        m_GLContext->DeleteTextures( 1, &m_TextureID );
        m_TextureID = 0;
    }

    if ( m_RBO != 0 )
    {
        m_GLContext->DeleteRenderbuffers( 1, &m_RBO );
        m_RBO = 0;
    }
}

void
HotReloadFrameBuffer::CreateFrameBuffer( float Width, float Height )
{
    const auto* gl = m_GLContext;
    GL_CHECK( gl->GenFramebuffers( 1, &m_FBO ) );
    GL_CHECK( gl->BindFramebuffer( GL_FRAMEBUFFER, m_FBO ) );

    GL_CHECK( gl->GenTextures( 1, &m_TextureID ) );
    GL_CHECK( gl->BindTexture( GL_TEXTURE_2D, m_TextureID ) );

    spdlog::info( "Creating FrameBuffer: {}x{} of format GL_RGB", Width, Height );
    GL_CHECK( gl->TexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr ) );
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
    GL_CHECK( gl->FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0 ) );

    GL_CHECK( gl->GenRenderbuffers( 1, &m_RBO ) );
    GL_CHECK( gl->BindRenderbuffer( GL_RENDERBUFFER, m_RBO ) );
    GL_CHECK( gl->RenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height ) );
    GL_CHECK( gl->FramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO ) );

    if ( gl->CheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
        spdlog::error( "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" );

    GL_CHECK( gl->BindFramebuffer( GL_FRAMEBUFFER, 0 ) );
    GL_CHECK( gl->BindTexture( GL_TEXTURE_2D, 0 ) );
    GL_CHECK( gl->BindRenderbuffer( GL_RENDERBUFFER, 0 ) );
}

void
HotReloadFrameBuffer::BindFrameBuffer( )
{
    m_GLContext->GetIntegerv( GL_FRAMEBUFFER_BINDING, &m_PreviousFBO );
    m_GLContext->BindFramebuffer( GL_FRAMEBUFFER, m_FBO );
}

void
HotReloadFrameBuffer::UnBindFrameBuffer( )
{
    m_GLContext->BindFramebuffer( GL_FRAMEBUFFER, m_PreviousFBO );
    m_PreviousFBO = 0;
}

void
HotReloadFrameBuffer::RescaleFrameBuffer( float Width, float Height )
{
    const auto* gl = m_GLContext;
    GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

    GL_CHECK( gl->BindTexture( GL_TEXTURE_2D, m_TextureID ) );
    GL_CHECK( gl->TexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr ) );
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
    GL_CHECK( gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
    GL_CHECK( gl->FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0 ) );

    GL_CHECK( gl->BindRenderbuffer( GL_RENDERBUFFER, m_RBO ) );
    GL_CHECK( gl->RenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height ) );
    GL_CHECK( gl->FramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO ) );
}

uint32_t
HotReloadFrameBuffer::GetTextureID( ) const
{
    return m_TextureID;
}

void
HotReloadFrameBuffer::SetGLContext( struct GladGLContext* Context )
{
    m_GLContext = Context;
}
