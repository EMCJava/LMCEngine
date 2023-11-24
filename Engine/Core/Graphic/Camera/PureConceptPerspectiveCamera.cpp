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
    const glm::mat4 Projection = glm::perspective(
        glm::radians( m_CameraPerspectiveFOV ),
        m_CameraWidth / m_CameraHeight,
        m_CameraPerspectiveNear,
        m_CameraPerspectiveFar );

    const glm::mat4 View = glm::lookAt( m_CameraPosition, m_CameraPosition + m_CameraFrontVec, m_CameraUpVec );

    m_ProjectionMatrix = Projection * View;
}

void
PureConceptPerspectiveCamera::SetCameraPosition( const glm::vec3& Position, bool UpdateMatrix )
{
    m_CameraPosition = Position;
    if ( UpdateMatrix )
    {
        UpdateProjectionMatrix( );
    }
}

void
PureConceptPerspectiveCamera::SetCameraFacing( const glm::vec3& Vector, bool UpdateMatrix )
{
    m_CameraFrontVec = Vector;
    m_CameraRightVec = glm::normalize( CalculateCameraRightVector( ) );

    if ( UpdateMatrix )
    {
        UpdateProjectionMatrix( );
    }
}

void
PureConceptPerspectiveCamera::SetCameraUpVector( const glm::vec3& Vector, bool UpdateMatrix )
{
    m_CameraUpVec    = Vector;
    m_CameraRightVec = glm::normalize( CalculateCameraRightVector( ) );

    if ( UpdateMatrix )
    {
        UpdateProjectionMatrix( );
    }
}

void
PureConceptPerspectiveCamera::SetCameraPerspectiveFOV( FloatTy FOV, bool UpdateMatrix )
{
    m_CameraPerspectiveFOV = FOV;
    if ( UpdateMatrix )
    {
        UpdateProjectionMatrix( );
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
    m_CameraViewingYaw += Yaw;
    m_CameraViewingPitch = glm::clamp( m_CameraViewingPitch + Pitch, -89.f, 89.f );

    SetCameraFacing(
        glm::vec3 { cos( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ) },
        true );
}

void
PureConceptPerspectiveCamera::SetCameraPrincipalAxes( FloatTy Yaw, FloatTy Pitch )
{
    m_CameraViewingYaw = Yaw;
    m_CameraViewingPitch = Pitch;

    SetCameraFacing(
        glm::vec3 { cos( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingPitch ) ),
                    sin( glm::radians( m_CameraViewingYaw ) ) * cos( glm::radians( m_CameraViewingPitch ) ) },
        true );
}
