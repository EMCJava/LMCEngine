//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <spdlog/fmt/fmt.h>

class OrientationRotation
{
public:
    struct Rotation {
        FloatTy X = 0, Y = 0, Z = 0;
    };

    const Rotation&
    GetRotation( ) const;

    Rotation&
    GetRotation( );

protected:
    Rotation m_Rotation { };
};

template <>
struct fmt::formatter<OrientationRotation::Rotation> : fmt::formatter<std::string> {
    static auto
    format( const OrientationRotation::Rotation& Rotation, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Rotation X={} Y={} Z={}]", Rotation.X, Rotation.Y, Rotation.Z );
    }
};

template <>
struct fmt::formatter<OrientationRotation> : fmt::formatter<std::string> {
    static auto
    format( const OrientationRotation& ORotation, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::formatter<OrientationRotation::Rotation>::format( ORotation.GetRotation( ), ctx );
    }
};
