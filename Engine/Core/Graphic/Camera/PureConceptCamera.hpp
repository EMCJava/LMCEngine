//
// Created by loyus on 8/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>
#include <Engine/Core/Concept/PureConcept.hpp>

#include <glm/glm.hpp>

class PureConceptCamera : public PureConcept
{
    DECLARE_CONCEPT(PureConceptCamera, PureConcept)

public:
    [[nodiscard]] const glm::mat4&
    GetProjectionMatrix( ) const;

    void SetDimensions( int Width, int Height );

private:
    glm::mat4 m_ProjectionMatrix;
    FloatTy   m_CameraWidth { }, m_CameraHeight { };
};
