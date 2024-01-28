//
// Created by samsa on 1/10/2024.
//

#include "OrientationMatrix.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <new>

DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( OrientationMatrix, Orientation )

glm::vec3
OrientationMatrix::GetWorldCoordinate( ) const
{
    return glm::vec3 { m_ModelMatrix[ 3 ] };
}

auto
OrientationMatrix::GetCoordinate( ) const -> const glm::vec3&
{
    return Coordinate;
}

const glm::vec3&
OrientationMatrix::AlterCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Coordinate.x += X;
    Coordinate.y += Y;
    Coordinate.z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return Coordinate;
}

const glm::vec3&
OrientationMatrix::AlterCoordinate( const glm::vec3& DeltaCoordinate, bool UpdateMatrix )
{
    Coordinate += DeltaCoordinate;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return Coordinate;
}

const glm::vec3&
OrientationMatrix::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Coordinate.x = X;
    Coordinate.y = Y;
    Coordinate.z = Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return Coordinate;
}

const glm::vec3&
OrientationMatrix::AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    TranslationOrigin.x += X;
    TranslationOrigin.y += Y;
    TranslationOrigin.z += Z;

    RotationOrigin.x += X;
    RotationOrigin.y += Y;
    RotationOrigin.z += Z;

    ScaleOrigin.x += X;
    ScaleOrigin.y += Y;
    ScaleOrigin.z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return TranslationOrigin;
}

const glm::vec3&
OrientationMatrix::SetOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Orientation::SetOrigin( X, Y, Z );

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return TranslationOrigin;
}

const glm::vec3&
OrientationMatrix::SetTranslationOrigin( const glm::vec3& TranslationOrigin, bool UpdateMatrix )
{
    this->TranslationOrigin += TranslationOrigin;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return this->TranslationOrigin;
}

const glm::vec3&
OrientationMatrix::SetRotationOrigin( const glm::vec3& RotationOrigin, bool UpdateMatrix )
{
    this->RotationOrigin = RotationOrigin;

    SameRotationScaleOrigin = RotationOrigin == ScaleOrigin;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return this->RotationOrigin;
}

const glm::vec3&
OrientationMatrix::SetScaleOrigin( const glm::vec3& ScaleOrigin, bool UpdateMatrix )
{
    this->ScaleOrigin = ScaleOrigin;

    SameRotationScaleOrigin = RotationOrigin == ScaleOrigin;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return this->ScaleOrigin;
}

glm::mat4&
OrientationMatrix::GetTranslationMatrix( )
{
    return m_TranslationMatrix;
}

void
OrientationMatrix::UpdateTranslationMatrix( )
{
    m_TranslationMatrix = glm::translate( glm::mat4( 1 ), Coordinate - TranslationOrigin );
}

const glm::vec3&
OrientationMatrix::SetCoordinate( const glm::vec3& Coor, bool UpdateMatrix )
{
    Coordinate = Coor;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return Coordinate;
}

const glm::vec3&
OrientationMatrix::AlterRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Orientation::AlterRotation( X, Y, Z );

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Rotation;
}


const glm::vec3&
OrientationMatrix::SetRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Orientation::SetRotation( X, Y, Z );

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Rotation;
}

const glm::vec3&
OrientationMatrix::SetRotation( const glm::vec3& Rotation, bool UpdateMatrix )
{
    return SetRotation( Rotation.x, Rotation.y, Rotation.z, UpdateMatrix );
}

glm::mat4&
OrientationMatrix::GetRotationMatrix( )
{
    return m_RotationMatrix;
}

void
OrientationMatrix::UpdateRotationMatrix( )
{
    if ( m_UsingQuat )
    {
        m_RotationMatrix = glm::translate( glm::translate( glm::mat4( 1 ), RotationOrigin )
                                               * glm::mat4_cast( m_Quat ),
                                           -RotationOrigin );
    } else
    {
        m_RotationMatrix = glm::translate( glm::translate( glm::mat4( 1 ), RotationOrigin )
                                               * glm::eulerAngleXYZ( m_Rotation.x,
                                                                     m_Rotation.y,
                                                                     m_Rotation.z ),
                                           -RotationOrigin );
    }
}

const glm::vec3&
OrientationMatrix::SetScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Scale.x = X;
    Scale.y = Y;
    Scale.z = Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return Scale;
}

const glm::vec3&
OrientationMatrix::SetScale( const glm::vec3& Scale, bool UpdateMatrix )
{
    this->Scale = Scale;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return Scale;
}

const glm::vec3&
OrientationMatrix::AlterScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    Scale.x += X;
    Scale.y += Y;
    Scale.z += Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return Scale;
}

const glm::vec3&
OrientationMatrix::GetScale( ) const
{
    return Scale;
}

void
OrientationMatrix::UpdateScaleMatrix( )
{
    m_ScaleMatrix = glm::translate( glm::mat4( 1 ), ScaleOrigin );
    m_ScaleMatrix = glm::scale( m_ScaleMatrix, Scale );
    m_ScaleMatrix = glm::translate( m_ScaleMatrix, -ScaleOrigin );
}

glm::mat4&
OrientationMatrix::GetScaleMatrix( )
{
    return m_ScaleMatrix;
}

void
OrientationMatrix::UpdateModelMatrix( )
{
    m_ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
}

void
OrientationMatrix::RecalculateModelMatrix( )
{
    UpdateTranslationMatrix( );
    UpdateRotationMatrix( );
    UpdateRotationMatrix( );

    UpdateModelMatrix( );
}

const glm::mat4&
OrientationMatrix::GetModelMatrix( ) const
{
    return m_ModelMatrix;
}

void
OrientationMatrix::CalculateModelMatrix( glm::mat4* Result ) const
{
    CalculateModelMatrix( Result, static_cast<const Orientation&>( *this ) );
}

void
OrientationMatrix::CalculateModelMatrix( glm::mat4* Result, const Orientation& Ori )
{
    // Construct inplace (set as identical matrix)
    new ( Result ) glm::mat4( 1 );

    *Result = glm::translate( *Result, ( Ori.Coordinate - Ori.TranslationOrigin ) + Ori.RotationOrigin );

    assert( offsetof( OrientationMatrix, m_TranslationMatrix ) == sizeof( Orientation ) );
    const auto& OriMat = static_cast<const OrientationMatrix&>( Ori );

    if ( OriMat.m_UsingQuat )
        *Result = *Result * glm::eulerAngleXYZ( OriMat.m_Rotation.x, OriMat.m_Rotation.y, OriMat.m_Rotation.z );
    else
        *Result = *Result * glm::mat4_cast( OriMat.m_Quat );

    if ( !Ori.SameRotationScaleOrigin )
    {
        *Result = glm::translate( *Result, -Ori.RotationOrigin + Ori.ScaleOrigin );
    }

    *Result = glm::scale( *Result, Ori.Scale );
    *Result = glm::translate( *Result, -Ori.ScaleOrigin );
}

const glm::quat&
OrientationMatrix::SetQuat( const glm::quat& Quat, bool UpdateMatrix )
{
    Orientation::SetQuat( Quat );

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return this->m_Quat;
}

OrientationMatrix&
OrientationMatrix::operator+=( const Orientation& Other )
{
    Orientation::operator+=( Other );
    RecalculateModelMatrix( );
    return *this;
}
