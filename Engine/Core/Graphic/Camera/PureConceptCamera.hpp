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
{
    DECLARE_CONCEPT( PureConceptCamera, PureConcept )

public:
    [[nodiscard]] const glm::mat4&
    GetProjectionMatrix( ) const;

    void SetDimensions( int Width, int Height );
    void SetScale( FloatTy Scale );

    void UpdateProjectionMatrix( );

private:
    glm::mat4 m_ProjectionMatrix { 1 };
    FloatTy   m_CameraWidth { }, m_CameraHeight { };
    FloatTy   m_Scale { };
};
