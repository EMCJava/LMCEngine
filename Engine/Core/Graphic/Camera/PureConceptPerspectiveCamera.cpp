//
// Created by samsa on 10/19/2023.
//

#include "PureConceptPerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( PureConceptPerspectiveCamera )

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
    m_CameraRightVec = glm::normalize( glm::cross( m_CameraFrontVec, m_CameraUpVec ) );

    if ( UpdateMatrix )
    {
        UpdateProjectionMatrix( );
    }
}

void
PureConceptPerspectiveCamera::SetCameraUpVector( const glm::vec3& Vector, bool UpdateMatrix )
{
    m_CameraUpVec    = Vector;
    m_CameraRightVec = glm::normalize( glm::cross( m_CameraFrontVec, m_CameraUpVec ) );

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
    return glm::cross( m_CameraFrontVec, m_CameraUpVec );
}
