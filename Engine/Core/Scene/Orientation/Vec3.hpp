//
// Created by samsa on 9/22/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <spdlog/fmt/fmt.h>

struct Vec3 {
    FloatTy X = 0, Y = 0, Z = 0;

    inline explicit Vec3( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );
    inline Vec3( const Vec3& other );
    inline Vec3 operator+( const Vec3& other ) const;
    inline Vec3 operator*( const FloatTy& other ) const;
};

Vec3::Vec3( FloatTy X, FloatTy Y, FloatTy Z )
    : X( X )
    , Y( Y )
    , Z( Z )
{ }

Vec3::Vec3( const Vec3& other )
{
    X = other.X;
    Y = other.Y;
    Z = other.Z;
}

Vec3
Vec3::operator+( const Vec3& other ) const
{
    return Vec3{ this->X + other.X, this->Y + other.Y, this->Z + other.Z };
}

Vec3
Vec3::operator*( const FloatTy& other ) const
{
    return Vec3{ this->X * other, this->Y * other, this->Z * other };
}

template <>
struct fmt::formatter<Vec3> : fmt::formatter<std::string> {
    static auto
    format( const Vec3& C, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate X={} Y={} Z={}]", C.X, C.Y, C.Z );
    }
};