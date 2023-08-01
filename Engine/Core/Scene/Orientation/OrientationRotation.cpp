//
// Created by loyus on 8/1/2023.
//

#include "OrientationRotation.hpp"

#include <glm/gtc/matrix_transform.hpp>

const OrientationRotation::Rotation&
OrientationRotation::GetRotation( ) const
{
    return m_Rotation;
}

const OrientationRotation::Rotation&
OrientationRotation::AlterRotation( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Rotation.X += X;
    m_Rotation.Y += Y;
    m_Rotation.Z += Z;

    UpdateRotationMatrix( );

    return m_Rotation;
}


const OrientationRotation::Rotation&
OrientationRotation::SetRotation( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Rotation.X = X;
    m_Rotation.Y = Y;
    m_Rotation.Z = Z;

    UpdateRotationMatrix( );

    return m_Rotation;
}

const OrientationCoordinate::Coordinate&
OrientationRotation::GetRotationCenter( ) const
{
    return m_RotationCenter;
}

const OrientationCoordinate::Coordinate&
OrientationRotation::SetRotationCenter( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_RotationCenter.X = X;
    m_RotationCenter.Y = Y;
    m_RotationCenter.Z = Z;

    UpdateRotationMatrix( );

    return m_RotationCenter;
}

glm::mat4&
OrientationRotation::GetRotationMatrix( )
{
    return m_RotationMatrix;
}

void
OrientationRotation::UpdateRotationMatrix( )
{
    m_RotationMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_RotationCenter.X, m_RotationCenter.Y, m_RotationCenter.Z ) );

    // Apply rotation on X-axis
    if ( m_Rotation.X != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.X, glm::vec3( 1.0f, 0.0f, 0.0f ) );

    // Apply rotation on Y-axis
    if ( m_Rotation.Y != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.Y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

    // Apply rotation on Z-axis
    if ( m_Rotation.Z != 0 )
        m_RotationMatrix = glm::rotate( m_RotationMatrix, m_Rotation.Z, glm::vec3( 0.0f, 0.0f, 1.0f ) );

    m_RotationMatrix = glm::translate( m_RotationMatrix, -glm::vec3( m_RotationCenter.X, m_RotationCenter.Y, m_RotationCenter.Z ) );
}
