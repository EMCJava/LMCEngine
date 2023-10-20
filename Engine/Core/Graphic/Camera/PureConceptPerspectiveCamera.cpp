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
PureConceptPerspectiveCamera::SetCameraPosition( const glm::vec3& Position )
{
    m_CameraPosition = Position;
    UpdateProjectionMatrix( );
}

void
PureConceptPerspectiveCamera::SetCameraFacing( const glm::vec3& Vector )
{
    m_CameraFrontVec = Vector;
    UpdateProjectionMatrix( );
}

void
PureConceptPerspectiveCamera::SetCameraUpVector( const glm::vec3& Vector )
{
    m_CameraUpVec = Vector;
    UpdateProjectionMatrix( );
}

void
PureConceptPerspectiveCamera::SetCameraPerspectiveFOV( FloatTy FOV )
{
    m_CameraPerspectiveFOV = FOV;
    UpdateProjectionMatrix( );
}
