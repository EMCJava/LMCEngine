//
// Created by loyus on 8/1/2023.
//

#pragma once

#include "Vec3.hpp"

#include <glm/glm.hpp>

class Orientation
{
public:
    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const Vec3&
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    SetCoordinate( Vec3 Coor, bool UpdateMatrix = true );
    const Vec3&
    AlterCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    AlterCoordinate( const Vec3& DeltaCoordinate, bool UpdateMatrix = true );
    const Vec3&
    GetCoordinate( ) const;

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const Vec3&
    SetOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    AlterOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );


    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const Vec3&
    SetRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    AlterRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    GetRotation( ) const;

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const Vec3&
    SetScale( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    AlterScale( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const Vec3&
    GetScale( ) const;

    const Vec3&
    SetTranslationOrigin( const Vec3& TranslationOrigin, bool UpdateMatrix = true );
    const Vec3&
    SetRotationOrigin( const Vec3& RotationOrigin, bool UpdateMatrix = true );
    const Vec3&
    SetScaleOrigin( const Vec3& ScaleOrigin, bool UpdateMatrix = true );


    void       UpdateTranslationMatrix( );
    glm::mat4& GetTranslationMatrix( );

    void       UpdateRotationMatrix( );
    glm::mat4& GetRotationMatrix( );

    void       UpdateScaleMatrix( );
    glm::mat4& GetScaleMatrix( );

    void       UpdateModelMatrix( );
    glm::mat4& GetModelMatrix( );

    Vec3
    GetWorldCoordinate( ) const;

protected:
    Vec3 m_TranslationOrigin { }, m_RotationOrigin { }, m_ScaleOrigin { };
    Vec3 m_Coordinate { }, m_Rotation { }, m_Scale { 1, 1, 1 };

    glm::mat4 m_TranslationMatrix { 1 }, m_RotationMatrix { 1 }, m_ScaleMatrix { 1 };
    glm::mat4 m_ModelMatrix { 1 };
};

template <>
struct fmt::formatter<Orientation> : fmt::formatter<std::string> {
    static auto
    format( const Orientation& O, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate:({}), Rotation:({}), Scale:({})]", O.GetCoordinate( ), O.GetRotation( ), O.GetScale( ) );
    }
};
