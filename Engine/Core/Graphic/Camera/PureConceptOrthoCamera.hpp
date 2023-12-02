//
// Created by samsa on 10/17/2023.
//

#pragma once

#include "PureConceptCamera.hpp"

class PureConceptOrthoCamera : public PureConceptCamera
{
    DECLARE_CONCEPT( PureConceptOrthoCamera, PureConceptCamera )
public:
    void
    UpdateProjectionMatrix( ) override;
    void
    UpdateViewMatrix( ) override { }
    void
    UpdateCameraMatrix( ) override;

    void
    SetCoordinate( FloatTy X, FloatTy Y, FloatTy = 0 );

    const glm::vec3&
    GetCoordinate( );

    void
    SetScale( FloatTy Scale );

    FloatTy
    GetScale( ) const;

    /*
     *
     * Coordinate system transform
     *
     * */
    void
    ScreenCoordToUICoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const;

    void
    ScreenCoordToWorldCoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const;

protected:
    FloatTy   m_Scale { 1 };
    glm::vec3 m_Coordinate;
};
