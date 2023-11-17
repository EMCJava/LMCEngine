//
// Created by loyus on 8/1/2023.
//

#include "Orientation.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <new>

glm::vec3
Orientation::GetWorldCoordinate( ) const
{
    const auto ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
    glm::vec3  WorldOffset { ModelMatrix[ 3 ] };

    return glm::vec3 { WorldOffset.x, WorldOffset.y, WorldOffset.z };
}

auto
Orientation::GetCoordinate( ) const -> const glm::vec3&
{
    return m_Coordinate;
}

const glm::vec3&
Orientation::AlterCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Coordinate.x += X;
    m_Coordinate.y += Y;
    m_Coordinate.z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const glm::vec3&
Orientation::AlterCoordinate( const glm::vec3& DeltaCoordinate, bool UpdateMatrix )
{
    m_Coordinate += DeltaCoordinate;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const glm::vec3&
Orientation::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Coordinate.x = X;
    m_Coordinate.y = Y;
    m_Coordinate.z = Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const glm::vec3&
Orientation::AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_TranslationOrigin.x += X;
    m_TranslationOrigin.y += Y;
    m_TranslationOrigin.z += Z;

    m_RotationOrigin.x += X;
    m_RotationOrigin.y += Y;
    m_RotationOrigin.z += Z;

    m_ScaleOrigin.x += X;
    m_ScaleOrigin.y += Y;
    m_ScaleOrigin.z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const glm::vec3&
Orientation::SetOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_TranslationOrigin.x = X;
    m_TranslationOrigin.y = Y;
    m_TranslationOrigin.z = Z;

    m_RotationOrigin = m_ScaleOrigin = m_TranslationOrigin;
    m_SameRotationScaleOrigin        = true;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const glm::vec3&
Orientation::SetTranslationOrigin( const glm::vec3& TranslationOrigin, bool UpdateMatrix )
{
    m_TranslationOrigin += TranslationOrigin;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const glm::vec3&
Orientation::SetRotationOrigin( const glm::vec3& RotationOrigin, bool UpdateMatrix )
{
    m_RotationOrigin = RotationOrigin;

    m_SameRotationScaleOrigin = m_RotationOrigin == m_ScaleOrigin;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_RotationOrigin;
}

const glm::vec3&
Orientation::SetScaleOrigin( const glm::vec3& ScaleOrigin, bool UpdateMatrix )
{
    m_ScaleOrigin = ScaleOrigin;

    m_SameRotationScaleOrigin = m_RotationOrigin == m_ScaleOrigin;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_ScaleOrigin;
}

glm::mat4&
Orientation::GetTranslationMatrix( )
{
    return m_TranslationMatrix;
}

void
Orientation::UpdateTranslationMatrix( )
{
    m_TranslationMatrix = glm::translate( glm::mat4( 1 ), m_Coordinate - m_TranslationOrigin );
}

const glm::vec3&
Orientation::SetCoordinate( glm::vec3 Coor, bool UpdateMatrix )
{
    m_Coordinate = Coor;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const glm::vec3&
Orientation::GetRotation( ) const
{
    return m_Rotation;
}

const glm::vec3&
Orientation::AlterRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    if ( m_UsingQuat )
    {
        ActivateEularRotation( );
        m_UsingQuat = false;
    }

    m_Rotation.x += X;
    m_Rotation.y += Y;
    m_Rotation.z += Z;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Rotation;
}


const glm::vec3&
Orientation::SetRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_UsingQuat = false;

    m_Rotation.x = X;
    m_Rotation.y = Y;
    m_Rotation.z = Z;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Rotation;
}

glm::mat4&
Orientation::GetRotationMatrix( )
{
    return m_RotationMatrix;
}

void
Orientation::UpdateRotationMatrix( )
{
    if ( m_UsingQuat )
    {
        m_RotationMatrix = glm::translate( glm::translate( glm::mat4( 1 ), m_RotationOrigin )
                                               * glm::mat4_cast( m_Quat ),
                                           -m_RotationOrigin );
    } else
    {
        m_RotationMatrix = glm::translate( glm::translate( glm::mat4( 1 ), m_RotationOrigin )
                                               * glm::eulerAngleXYZ( m_Rotation.x, m_Rotation.y, m_Rotation.z ),
                                           -m_RotationOrigin );
    }
}

const glm::vec3&
Orientation::SetScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Scale.x = X;
    m_Scale.y = Y;
    m_Scale.z = Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_Scale;
}

const glm::vec3&
Orientation::AlterScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Scale.x += X;
    m_Scale.y += Y;
    m_Scale.z += Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_Scale;
}

const glm::vec3&
Orientation::GetScale( ) const
{
    return m_Scale;
}

void
Orientation::UpdateScaleMatrix( )
{
    m_ScaleMatrix = glm::translate( glm::mat4( 1 ), m_ScaleOrigin );
    m_ScaleMatrix = glm::scale( m_ScaleMatrix, m_Scale );
    m_ScaleMatrix = glm::translate( m_ScaleMatrix, -m_ScaleOrigin );
}

glm::mat4&
Orientation::GetScaleMatrix( )
{
    return m_ScaleMatrix;
}

void
Orientation::UpdateModelMatrix( )
{
    m_ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
}

glm::mat4&
Orientation::GetModelMatrix( )
{
    return m_ModelMatrix;
}

void
Orientation::CalculateModelMatrix( glm::mat4* Result ) const
{
    // Construct inplace (set as identical matrix)
    new ( Result ) glm::mat4( 1 );

    *Result = glm::translate( *Result, ( m_Coordinate - m_TranslationOrigin ) + m_RotationOrigin );

    if ( m_UsingQuat )
        *Result = *Result * glm::eulerAngleXYZ( m_Rotation.x, m_Rotation.y, m_Rotation.z );
    else
        *Result = *Result * glm::mat4_cast( m_Quat );

    if ( !m_SameRotationScaleOrigin )
    {
        *Result = glm::translate( *Result, -m_RotationOrigin + m_ScaleOrigin );
    }

    *Result = glm::scale( *Result, m_Scale );
    *Result = glm::translate( *Result, -m_ScaleOrigin );
}

const glm::quat&
Orientation::SetQuat( const glm::quat& Quat, bool UpdateMatrix )
{
    m_UsingQuat = true;
    m_Quat      = Quat;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Quat;
}

void
Orientation::ActivateEularRotation( )
{
    if ( m_UsingQuat )
    {
        auto Copy   = m_Quat;
        m_Rotation  = eulerAngles( Copy );
        m_UsingQuat = false;
    }
}

void
Orientation::ActivateQuatRotation( )
{
    if ( !m_UsingQuat )
    {
        auto Copy   = m_Rotation;
        m_Quat      = glm::quat( Copy );
        m_UsingQuat = true;
    }
}
