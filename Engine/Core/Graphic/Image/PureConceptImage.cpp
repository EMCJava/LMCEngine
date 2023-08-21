//
// Created by loyus on 8/21/2023.
//

#include "PureConceptImage.hpp"
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <spdlog/spdlog.h>

#include <imgui.h>

#include <stb_image.h>

DEFINE_CONCEPT( PureConceptImage )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( PureConceptImage, PureConcept, m_FilePath )

PureConceptImage::~PureConceptImage( )
{
    spdlog::trace( "PureConceptImage::~PureConceptImage -> {}", fmt::ptr( this ) );
    ReleaseData( );
}

bool
PureConceptImage::LoadImage( )
{
    ReleaseData( );

    stbi_set_flip_vertically_on_load( true );   // tell stb_image.h to flip loaded texture's on the y-axis.
    m_ImageData = stbi_load( m_FilePath.c_str( ), &m_ImageWidth, &m_ImageHeight, &m_ImageChannelCount, 0 );
    if ( m_ImageData != nullptr )
    {
        GLenum Format;
        if ( m_ImageChannelCount == 1 )
        {
            Format = GL_RED;
        } else if ( m_ImageChannelCount == 3 )
        {
            Format = GL_RGB;
        } else if ( m_ImageChannelCount == 4 )
        {
            Format = GL_RGBA;
        }

        spdlog::info( "Loaded Image {} with size {} x {} C {}", m_FilePath, m_ImageWidth, m_ImageHeight, m_ImageChannelCount );
        return true;

    } else
    {
        spdlog::error( "Failed to load texture" );
    }

    return false;
}

void
PureConceptImage::ReleaseData( )
{
    if ( m_ImageData == nullptr ) return;

    stbi_image_free( m_ImageData );
    m_ImageData = nullptr;
}

PureConceptImage::PureConceptImage( const std::string& FilePath )
{
    m_FilePath = FilePath;
    LoadImage( );
}

bool
PureConceptImage::LoadImage( const std::string& FilePath )
{
    m_FilePath = FilePath;
    return LoadImage( );
}
