//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>

#include <glm/glm.hpp>

class PureConceptCamera : public PureConcept
    , public std::enable_shared_from_this<PureConceptCamera>
{
    DECLARE_CONCEPT( PureConceptCamera, PureConcept )

public:
    [[nodiscard]] const glm::mat4&
    GetProjectionMatrix( ) const;

    [[nodiscard]] const glm::mat4&
    GetProjectionMatrixNonOffset( ) const;

    void
    SetDimensions( int Width, int Height );
    std::pair<FloatTy, FloatTy>
    GetDimensions( ) const;

    void
    SetScale( FloatTy Scale );
    FloatTy
    GetScale( ) const;

    virtual void
    UpdateProjectionMatrix( );

    void
    ScreenCoordToUICoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const;

    void
    ScreenCoordToWorldCoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const;

    static PureConceptCamera*
    PeekCameraStack( );

    void
    PushToCameraStack( );

    void
    PopFromCameraStack( );

    void
    SetCoordinate( FloatTy X, FloatTy Y, FloatTy = 0 );

    const glm::vec3&
    GetCoordinate( );

private:
    glm::mat4 m_ProjectionMatrix { 1 };
    FloatTy   m_CameraWidth { }, m_CameraHeight { };

    ENABLE_IMGUI( PureConceptCamera )
};
