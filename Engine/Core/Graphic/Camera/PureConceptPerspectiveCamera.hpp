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
    void
    UpdateViewMatrix( ) override;
    void
    UpdateCameraMatrix( ) override;

    void
    SetCameraPerspectiveFOV( FloatTy FOV, bool UpdateMatrix = true );

    /*
     *
     * Camera Orientation
     *
     * */
    void
    SetCameraPosition( const glm::vec3& Position, bool UpdateMatrix = true );
    void
    SetCameraFacing( const glm::vec3& Vector, bool UpdateMatrix = true );
    void
    SetCameraUpVector( const glm::vec3& Vector, bool UpdateMatrix = true );

    inline const glm::vec3&
    GetCameraPosition( ) const { return m_CameraPosition; }
    inline const glm::vec3&
    GetCameraFacing( ) const { return m_CameraFrontVec; }
    inline const glm::vec3&
    GetCameraRightVector( ) const { return m_CameraRightVec; }
    inline const glm::vec3&
    GetCameraUpVector( ) const { return m_CameraUpVec; }

    glm::vec3
    CalculateCameraRightVector( ) const;

    void
    AlterCameraPrincipalAxes( FloatTy Yaw, FloatTy Pitch );
    void
    SetCameraPrincipalAxes( FloatTy Yaw, FloatTy Pitch );
    FloatTy
    GetCameraPrincipalYaw( ) const noexcept { return m_CameraViewingYaw; }
    FloatTy
    GetCameraPrincipalPitch( ) const noexcept { return m_CameraViewingPitch; }

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
    FloatTy m_CameraViewingYaw;
    FloatTy m_CameraViewingPitch;

    glm::vec3 m_CameraFrontVec { };

    glm::vec3 m_CameraRightVec { };
    glm::vec3 m_CameraUpVec { 0, 1, 0 };

    glm::vec3 m_CameraPosition { };

    ENABLE_IMGUI( PureConceptPerspectiveCamera )
};
