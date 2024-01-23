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
     * Only allow user to modify the orientation using setter for a easier life to update the matrix
     *
     * */
    const glm::vec3&
    SetRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );
    const glm::vec3&
    AlterRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0 );
    const glm::quat&
    SetQuat( const glm::quat& Quat );

    const glm::vec4&
    GetQuatVec4( ) const noexcept;
    const glm::quat&
    GetQuat( ) const noexcept;

    /*
     *
     * Transform quaternions to euler angles
     *
     * */
    void
    ActivateEularRotation( );

    /*
     *
     * Transform euler angles to quaternions
     *
     * */
    void
    ActivateQuatRotation( );

    bool IsUsingQuatRotation( ) const noexcept { return m_UsingQuat; }

    /*
     *
     * Modify three orientation at once
     *
     * */
    const glm::vec3&
    SetOrigin( FloatTy X, FloatTy Y, FloatTy Z );
    const glm::vec3&
    AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z );

    /*
     *
     * Get difference between two orientations
     *
     * */
    Orientation
    operator-( const Orientation& Other ) const;

    /*
     *
     * Apply difference between two orientations
     *
     * */
    Orientation&
    operator+=( const Orientation& Other );

public:
    glm::vec3 TranslationOrigin { }, RotationOrigin { }, ScaleOrigin { };
    glm::vec3 Coordinate { }, Scale { 1, 1, 1 };

protected:
    union
    {
        glm::vec3 m_Rotation;
        glm::vec4 m_QuatVec4;
        glm::quat m_Quat { };
    };

    bool m_UsingQuat = true;

public:
    /*
     *
     * Useful when calculating the final model matrix, save two translation
     *
     * */
    bool SameRotationScaleOrigin = false;

    ENABLE_IMGUI( Orientation )
};

template <>
struct fmt::formatter<glm::vec3> : fmt::formatter<std::string> {
    static auto
    format( const glm::vec3& C, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[{},{},{}]", C.x, C.y, C.z );
    }
};

template <>
struct fmt::formatter<glm::vec4> : fmt::formatter<std::string> {
    static auto
    format( const glm::vec4& C, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[{},{},{}, {}]", C.x, C.y, C.z, C.w );
    }
};
template <>
struct fmt::formatter<Orientation> : fmt::formatter<std::string> {
    static auto
    format( const Orientation& O, format_context& ctx ) -> decltype( ctx.out( ) )
    {
        return fmt::format_to( ctx.out( ), "[Coordinate:({}:{}), Rotation:({}:{}), Scale:({}:{})]", O.Coordinate, O.TranslationOrigin,
                               O.GetQuatVec4( ), O.RotationOrigin,
                               O.Scale, O.ScaleOrigin );
    }
};
