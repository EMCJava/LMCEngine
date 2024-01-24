//
// Created by samsa on 1/10/2024.
//

#pragma once

#include "Orientation.hpp"

class OrientationMatrix : protected Orientation
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
    SetRotation( const glm::vec3& Rotation, bool UpdateMatrix = true );

    const glm::vec3&
    AlterRotation( FloatTy X = 0, FloatTy Y = 0, FloatTy Z = 0, bool UpdateMatrix = true );
    const glm::quat&
    SetQuat( const glm::quat& Quat, bool UpdateMatrix = true );

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

    /*
     *
     * Recalculate only from other existing matrices
     *
     * */
    void UpdateModelMatrix( );

    /*
     *
     * Recalculate from start and update other cache matrices
     *
     * */
    void RecalculateModelMatrix( );

    const glm::mat4& GetModelMatrix( ) const;

    /*
     *
     * Calculate matrix realtime in place
     *
     * */
    void
    CalculateModelMatrix( glm::mat4* Result ) const;

    static void
    CalculateModelMatrix( glm::mat4* Result, const Orientation& Ori );

    glm::vec3
    GetWorldCoordinate( ) const;

    /*
     *
     * Orientation related
     *
     * */

    operator const Orientation&( ) const noexcept { return *this; }

    OrientationMatrix&
    operator+=( const Orientation& Other );

protected:
    glm::mat4 m_TranslationMatrix { 1 }, m_RotationMatrix { 1 }, m_ScaleMatrix { 1 };
    glm::mat4 m_ModelMatrix { 1 };

    ENABLE_IMGUI( OrientationMatrix )
};