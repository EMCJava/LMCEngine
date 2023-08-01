//
// Created by loyus on 7/10/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <spdlog/fmt/fmt.h>

class OrientationCoordinate
{
public:
    struct Coordinate {
        FloatTy X = 0, Y = 0, Z = 0;
    };

    const Coordinate&
    GetCoordinate( ) const;

    Coordinate&
    GetCoordinate( );

protected:
    Coordinate m_Coordinate { };
};

template <>
struct fmt::formatter<OrientationCoordinate::Coordinate> : fmt::formatter<std::string> {
    static auto
    format( const OrientationCoordinate::Coordinate& C, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate X={} Y={} Z={}]", C.X, C.Y, C.Z );
    }
};

template <>
struct fmt::formatter<OrientationCoordinate> : fmt::formatter<std::string> {
    static auto
    format( const OrientationCoordinate& OCoordinate, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::formatter<OrientationCoordinate::Coordinate>::format( OCoordinate.GetCoordinate( ), ctx );
    }
};
