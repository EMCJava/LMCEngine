//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>
#include <Engine/Core/Scene/Orientation/Orientation.hpp>

#include <glm/glm.hpp>

class PureConceptCamera : public PureConcept
    , public Orientation
    , public std::enable_shared_from_this<PureConceptCamera>
{
    DECLARE_CONCEPT( PureConceptCamera, PureConcept )

public:
    /*
     *
     * !!! First ever created camera will be set to the default camera for the engine !!!
     *
     * */
    PureConceptCamera( );

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

    void
    UpdateProjectionMatrix( );

    void
    ScreenCoordToUICoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const;

private:
    glm::mat4 m_ProjectionMatrix { 1 };
    glm::mat4 m_ProjectionMatrixNonOffset { 1 };
    FloatTy   m_CameraWidth { }, m_CameraHeight { };
    FloatTy   m_Scale { };

    ENABLE_IMGUI( PureConceptCamera )
};
