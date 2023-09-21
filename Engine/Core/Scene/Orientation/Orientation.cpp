//
// Created by loyus on 8/1/2023.
//

#include "Orientation.hpp"

#include <glm/gtc/matrix_transform.hpp>

Vec3
Orientation::GetWorldCoordinate( ) const
{
    const auto ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
    glm::vec3  WorldOffset { ModelMatrix[ 3 ] };

    return Vec3 { WorldOffset.x, WorldOffset.y, WorldOffset.z };
}

auto
Orientation::GetCoordinate( ) const -> const Vec3&
{
    return m_Coordinate;
}

const Vec3&
Orientation::AlterCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Coordinate.X += X;
    m_Coordinate.Y += Y;
    m_Coordinate.Z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const Vec3&
Orientation::AlterCoordinate( const Vec3& DeltaCoordinate, bool UpdateMatrix )
{
    m_Coordinate.X += DeltaCoordinate.X;
    m_Coordinate.Y += DeltaCoordinate.Y;
    m_Coordinate.Z += DeltaCoordinate.Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const Vec3&
Orientation::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Coordinate.X = X;
    m_Coordinate.Y = Y;
    m_Coordinate.Z = Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const Vec3&
Orientation::AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_TranslationOrigin.X += X;
    m_TranslationOrigin.Y += Y;
    m_TranslationOrigin.Z += Z;

    m_RotationOrigin.X += X;
    m_RotationOrigin.Y += Y;
    m_RotationOrigin.Z += Z;

    m_ScaleOrigin.X += X;
    m_ScaleOrigin.Y += Y;
    m_ScaleOrigin.Z += Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const Vec3&
Orientation::SetOrigin( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_TranslationOrigin.X = X;
    m_TranslationOrigin.Y = Y;
    m_TranslationOrigin.Z = Z;

    m_RotationOrigin = m_ScaleOrigin = m_TranslationOrigin;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateRotationMatrix( );
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const Vec3&
Orientation::SetTranslationOrigin( const Vec3& TranslationOrigin, bool UpdateMatrix )
{
    m_TranslationOrigin.X += TranslationOrigin.X;
    m_TranslationOrigin.Y += TranslationOrigin.Y;
    m_TranslationOrigin.Z += TranslationOrigin.Z;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_TranslationOrigin;
}

const Vec3&
Orientation::SetRotationOrigin( const Vec3& RotationOrigin, bool UpdateMatrix )
{
    m_RotationOrigin.X = RotationOrigin.X;
    m_RotationOrigin.Y = RotationOrigin.Y;
    m_RotationOrigin.Z = RotationOrigin.Z;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_RotationOrigin;
}

const Vec3&
Orientation::SetScaleOrigin( const Vec3& ScaleOrigin, bool UpdateMatrix )
{
    m_ScaleOrigin.X = ScaleOrigin.X;
    m_ScaleOrigin.Y = ScaleOrigin.Y;
    m_ScaleOrigin.Z = ScaleOrigin.Z;

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
    m_TranslationMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_Coordinate.X, m_Coordinate.Y, m_Coordinate.Z ) - glm::vec3( m_TranslationOrigin.X, m_TranslationOrigin.Y, m_TranslationOrigin.Z ) );
}

const Vec3&
Orientation::SetCoordinate( Vec3 Coor, bool UpdateMatrix )
{
    m_Coordinate = Coor;

    if ( UpdateMatrix )
    {
        UpdateTranslationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Coordinate;
}

const Vec3&
Orientation::GetRotation( ) const
{
    return m_Rotation;
}

const Vec3&
Orientation::AlterRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Rotation.X += X;
    m_Rotation.Y += Y;
    m_Rotation.Z += Z;

    if ( UpdateMatrix )
    {
        UpdateRotationMatrix( );
        UpdateModelMatrix( );
    }

    return m_Rotation;
}


const Vec3&
Orientation::SetRotation( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Rotation.X = X;
    m_Rotation.Y = Y;
    m_Rotation.Z = Z;

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
    m_RotationMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_RotationOrigin.X, m_RotationOrigin.Y, m_RotationOrigin.Z ) );

    // Apply rotation on X-axis
    if ( m_Rotation.X != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.X, glm::vec3( 1.0f, 0.0f, 0.0f ) );

    // Apply rotation on Y-axis
    if ( m_Rotation.Y != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.Y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

    // Apply rotation on Z-axis
    if ( m_Rotation.Z != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.Z, glm::vec3( 0.0f, 0.0f, 1.0f ) );

    m_RotationMatrix = glm::translate( m_RotationMatrix, -glm::vec3( m_RotationOrigin.X, m_RotationOrigin.Y, m_RotationOrigin.Z ) );
}

const Vec3&
Orientation::SetScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Scale.X = X;
    m_Scale.Y = Y;
    m_Scale.Z = Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_Scale;
}

const Vec3&
Orientation::AlterScale( FloatTy X, FloatTy Y, FloatTy Z, bool UpdateMatrix )
{
    m_Scale.X += X;
    m_Scale.Y += Y;
    m_Scale.Z += Z;

    if ( UpdateMatrix )
    {
        UpdateScaleMatrix( );
        UpdateModelMatrix( );
    }

    return m_Scale;
}

const Vec3&
Orientation::GetScale( ) const
{
    return m_Scale;
}

void
Orientation::UpdateScaleMatrix( )
{
    m_ScaleMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_ScaleOrigin.X, m_ScaleOrigin.Y, m_ScaleOrigin.Z ) );
    m_ScaleMatrix = glm::scale( m_ScaleMatrix, glm::vec3( m_Scale.X, m_Scale.Y, m_Scale.Z ) );
    m_ScaleMatrix = glm::translate( m_ScaleMatrix, -glm::vec3( m_ScaleOrigin.X, m_ScaleOrigin.Y, m_ScaleOrigin.Z ) );
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
