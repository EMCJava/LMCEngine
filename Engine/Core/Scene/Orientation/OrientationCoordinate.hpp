//
// Created by loyus on 7/10/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <spdlog/fmt/fmt.h>

#include <glm/glm.hpp>

class OrientationCoordinate
{
public:
    struct Coordinate {
        FloatTy X = 0, Y = 0, Z = 0;
    };

    const Coordinate&
    GetCoordinate( ) const;

    const Coordinate&
    AlterCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const Coordinate&
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    const Coordinate&
    GetOrigin( ) const;

    const Coordinate&
    AlterOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const Coordinate&
    SetOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );

    glm::mat4& GetTranslationMatrix( );
    void       UpdateTranslationMatrix( );

protected:
    Coordinate m_Coordinate { }, m_Origin { };
    glm::mat4  m_TranslationMatrix { glm::mat4( 1 ) };
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
