//
// Created by samsa on 10/17/2023.
//

#pragma once

#include "PureConceptCamera.hpp"

class PureConceptOrthoCamera : public PureConceptCamera
{
    DECLARE_CONCEPT( PureConceptOrthoCamera, PureConceptCamera )

protected:
    FloatTy   m_Scale { 1 };
    glm::vec3 m_Coordinate;
};
