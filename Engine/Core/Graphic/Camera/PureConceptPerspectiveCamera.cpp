//
// Created by samsa on 10/19/2023.
//

#include "PureConceptPerspectiveCamera.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( PureConceptPerspectiveCamera )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( PureConceptPerspectiveCamera, PureConceptCamera,
                                  m_CameraPerspectiveFOV, m_CameraPerspectiveNear, m_CameraPerspectiveFar,
                                  m_CameraViewingYaw, m_CameraViewingPitch,
                                  m_CameraPosition, m_CameraFrontVec, m_CameraRightVec, m_CameraUpVec )

void
PureConceptPerspectiveCamera::UpdateProjectionMatrix( )
{
    m_ProjectionMatrix = glm::perspective(
        glm::radians( m_CameraPerspectiveFOV ),
        m_CameraWidth / m_CameraHeight,
        m_CameraPerspectiveNear,
        m_CameraPerspectiveFar );
}

void
PureConceptPerspectiveCamera::UpdateViewMatrix( )
{
    m_ViewMatrix = glm::lookAt( m_CameraPosition, m_CameraPosition + m_CameraFrontVec, m_CameraUpVec );
}
void
PureConceptPerspectiveCamera::UpdateCameraMatrix( )
{
    PureConceptPerspectiveCamera::UpdateProjectionMatrix( );
    PureConceptPerspectiveCamera::UpdateViewMatrix( );
    m_CameraMatrixCache = m_ProjectionMatrix * m_ViewMatrix;
}

void
PureConceptPerspectiveCamera::SetCameraPosition( const glm::vec3& Position, bool UpdateMatrix )
{
    m_CameraPosition = Position;
    if ( UpdateMatrix )
    {
        PureConceptPerspectiveCamera::UpdateCameraMatrix( );
        m_CameraMatrixCache = m_ProjectionMatrix * m_ViewMatrix;
    }
}

void
PureConceptPerspectiveCamera::SetCameraFacing( const glm::vec3& Vector, bool UpdateMatrix )
{
    m_CameraFrontVec = glm::normalize( Vector );
    m_CameraRightVec = glm::normalize( CalculateCameraRightVector( ) );

    if ( UpdateMatrix )
    {
        PureConceptPerspectiveCamera::UpdateCameraMatrix( );
        m_CameraMatrixCache = m_ProjectionMatrix * m_ViewMatrix;
    }
}

void
PureConceptPerspectiveCamera::SetCameraUpVector( const glm::vec3& Vector, bool UpdateMatrix )
{
    m_CameraUpVec    = Vector;
    m_CameraRightVec = glm::normalize( CalculateCameraRightVector( ) );

    if ( UpdateMatrix )
    {
        PureConceptPerspectiveCamera::UpdateCameraMatrix( );
        m_CameraMatrixCache = m_ProjectionMatrix * m_ViewMatrix;
    }
}

void
PureConceptPerspectiveCamera::SetCameraPerspectiveFOV( FloatTy FOV, bool UpdateMatrix )
{
    m_CameraPerspectiveFOV = FOV;
    if ( UpdateMatrix )
    {
        PureConceptPerspectiveCamera::UpdateCameraMatrix( );
        m_CameraMatrixCache = m_ProjectionMatrix * m_ViewMatrix;
    }
}

glm::vec3
PureConceptPerspectiveCamera::CalculateCameraRightVector( ) const
{
    return glm::normalize( glm::cross( m_CameraFrontVec, m_CameraUpVec ) );
}

void
PureConceptPerspectiveCamera::AlterCameraPrincipalAxes( FloatTy Yaw, FloatTy Pitch )
{
    SetCameraPrincipalAxes( m_CameraViewingYaw + Yaw, glm::clamp( m_CameraViewingPitch + Pitch, -89.f, 89.f ) );
}

void
PureConceptPerspectiveCamera::SetCameraPrincipalAxes( FloatTy Yaw, FloatTy Pitch )
{
    m_CameraViewingYaw   = Yaw;
    m_CameraViewingPitch = Pitch;

    SetCameraFacing(
        glm::vec3 { cos( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ) },
        true );
}

RayCast::Ray
PureConceptPerspectiveCamera::ScreenSpaceToWorldSpaceRay( const glm::vec2& ScreenSpacePoint, FloatTy Distance ) const
{
    // glm::unProject
    const auto CameraPVInverse = inverse( m_CameraMatrixCache );

    glm::vec4 ScreenSpace = glm::vec4( ScreenSpacePoint.x, m_CameraHeight - ScreenSpacePoint.y, 1, 1 );
    ScreenSpace.x         = ScreenSpace.x / m_CameraWidth;
    ScreenSpace.y         = ScreenSpace.y / m_CameraHeight;
    ScreenSpace           = ScreenSpace * 2.f - 1.f;

    const auto WorldSpace = CameraPVInverse * ScreenSpace;

    const glm::vec3 WorldPosition = WorldSpace / WorldSpace.w;
    const glm::vec3 RayDirection  = glm::normalize( WorldPosition - m_CameraPosition );

    return { m_CameraPosition, RayDirection, Distance };
}
