//
// Created by loyus on 8/21/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include <string_view>

class PureConceptImage : public PureConcept
{
    DECLARE_CONCEPT( PureConceptImage, PureConcept )
public:
    PureConceptImage( ) = default;
    PureConceptImage( const std::string& FilePath );

    bool
    LoadImage( );

    bool
    LoadImage( const std::string& FilePath );

    void
    ReleaseData( );

    std::pair<int32_t, int32_t>
    GetImageDimension( ) const { return { m_ImageWidth, m_ImageHeight }; }

    int32_t
    GetImageChannelCount( ) const { return m_ImageChannelCount; }

    const auto*
    GetImageData( ) const { return m_ImageData; }

    const auto&
    GetImagePath( ) const { return m_FilePath; }

protected:
    std::string m_FilePath;
    int32_t     m_ImageWidth { }, m_ImageHeight { }, m_ImageChannelCount { };

    unsigned char* m_ImageData = nullptr;

    ENABLE_IMGUI( PureConceptImage )
};
