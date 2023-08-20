//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "OrientationCoordinate.hpp"
#include "OrientationRotation.hpp"

class Orientation : public OrientationCoordinate
    , public OrientationRotation
{
public:
    OrientationCoordinate::Coordinate
    GetWorldCoordinate( ) const;
};

template <>
struct fmt::formatter<Orientation> : fmt::formatter<std::string> {
    static auto
    format( const Orientation& O, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[({}), ({})]", O.GetCoordinate( ), O.GetRotation( ) );
    }
};
