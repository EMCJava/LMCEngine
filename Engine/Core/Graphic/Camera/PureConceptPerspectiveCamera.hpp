//
// Created by samsa on 10/19/2023.
//

#pragma once

#include "PureConceptCamera.hpp"

class PureConceptPerspectiveCamera : public PureConceptCamera
{
    DECLARE_CONCEPT( PureConceptPerspectiveCamera, PureConceptCamera )
public:
    void
    UpdateProjectionMatrix( ) override;

    /*
     *
     * Camera Orientation
     *
     * */
    void
    SetCameraPosition( const glm::vec3& Position );
    void
    SetCameraFacing( const glm::vec3& Vector );
    void
    SetCameraUpVector( const glm::vec3& Vector );

protected:
    // In degrees
    FloatTy m_CameraPerspectiveFOV { 45.0 };

    FloatTy m_CameraPerspectiveNear { 0.1f };
    FloatTy m_CameraPerspectiveFar { 1000.0f };

    /*
     *
     * Camera Orientation
     *
     * */
    glm::vec3 m_CameraPosition { };
    glm::vec3 m_CameraFrontVec { };
    glm::vec3 m_CameraUpVec { };

    ENABLE_IMGUI( PureConceptPerspectiveCamera )
};
