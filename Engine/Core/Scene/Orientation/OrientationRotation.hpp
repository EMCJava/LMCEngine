//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include "OrientationCoordinate.hpp"

#include <spdlog/fmt/fmt.h>

#include <glm/glm.hpp>

class OrientationRotation
{
public:
    struct Rotation {
        FloatTy X = 0, Y = 0, Z = 0;
    };

    const Rotation&
    GetRotation( ) const;

    const Rotation&
    AlterRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const Rotation&
    SetRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    const OrientationCoordinate::Coordinate&
    GetRotationCenter( ) const;

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const OrientationCoordinate::Coordinate&
    SetRotationCenter( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    glm::mat4& GetRotationMatrix( );
    void       UpdateRotationMatrix( );

protected:
    Rotation                          m_Rotation { };
    OrientationCoordinate::Coordinate m_RotationCenter { };
    glm::mat4                         m_RotationMatrix { };
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
        const auto& Rotation = ORotation.GetRotation( );

        return fmt::format_to( ctx.out( ), "[Rotation X={} Y={} Z={}, RotationCenter: {}]", Rotation.X, Rotation.Y, Rotation.Z, ORotation.GetRotationCenter( ) );
    }
};
