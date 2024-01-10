//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <spdlog/fmt/fmt.h>

class Orientation
{
public:
    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the marrix
     *
     * */
    const glm::vec3&
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    SetCoordinate( const glm::vec3&, bool UpdateMatrix = true );
    const glm::vec3&
    AlterCoordinate( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    AlterCoordinate( const glm::vec3& DeltaCoordinate, bool UpdateMatrix = true );
    const glm::vec3&
    GetCoordinate( ) const;

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const glm::vec3&
    SetOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    AlterOrigin( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );


    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const glm::vec3&
    SetRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    AlterRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    GetRotation( ) const;

    const glm::quat&
    SetQuat( const glm::quat& Quat, bool UpdateMatrix = true );

    void
    ActivateEularRotation( );

    void
    ActivateQuatRotation( );

    /*
     *
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const glm::vec3&
    SetScale( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    AlterScale( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::vec3&
    GetScale( ) const;

    const glm::vec3&
    SetTranslationOrigin( const glm::vec3& TranslationOrigin, bool UpdateMatrix = true );
    const glm::vec3&
    SetRotationOrigin( const glm::vec3& RotationOrigin, bool UpdateMatrix = true );
    const glm::vec3&
    SetScaleOrigin( const glm::vec3& ScaleOrigin, bool UpdateMatrix = true );


    void       UpdateTranslationMatrix( );
    glm::mat4& GetTranslationMatrix( );

    void       UpdateRotationMatrix( );
    glm::mat4& GetRotationMatrix( );

    void       UpdateScaleMatrix( );
    glm::mat4& GetScaleMatrix( );

    void             UpdateModelMatrix( );
    const glm::mat4& GetModelMatrix( ) const;

    /*
     *
     * Calculate matrix realtime in place
     *
     * */
    void
    CalculateModelMatrix( glm::mat4* Result ) const;

    glm::vec3
    GetWorldCoordinate( ) const;

protected:
    glm::vec3 m_TranslationOrigin { }, m_RotationOrigin { }, m_ScaleOrigin { };
    glm::vec3 m_Coordinate { }, m_Scale { 1, 1, 1 };

    union
    {
        glm::vec3 m_Rotation;
        glm::quat m_Quat { };
    };

    glm::mat4 m_TranslationMatrix { 1 }, m_RotationMatrix { 1 }, m_ScaleMatrix { 1 };
    glm::mat4 m_ModelMatrix { 1 };

    bool m_UsingQuat = true;

    /*
     *
     * Useful when calculating the final model matrix, save two translation
     *
     * */
    bool m_SameRotationScaleOrigin = false;

    ENABLE_IMGUI( Orientation )
};

template <>
struct fmt::formatter<glm::vec3> : fmt::formatter<std::string> {
    static auto
    format( const glm::vec3& C, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate X={} Y={} Z={}]", C.x, C.y, C.z );
    }
};
template <>
struct fmt::formatter<Orientation> : fmt::formatter<std::string> {
    static auto
    format( const Orientation& O, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate:({}), Rotation:({}), Scale:({})]", O.GetCoordinate( ), O.GetRotation( ), O.GetScale( ) );
    }
};
