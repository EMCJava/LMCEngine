//
// Created by loyus on 8/1/2023.
//

#include "PureConceptCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS(PureConceptCamera, PureConcept)

void
PureConceptCamera::SetDimensions( int Width, int Height )
{
    m_CameraWidth      = Width;
    m_CameraHeight     = Height;
    m_ProjectionMatrix = glm::ortho<FloatTy>( 0, m_CameraWidth, 0, m_CameraHeight, -1, 1 );
}

const glm::mat4&
PureConceptCamera::GetProjectionMatrix( ) const
{
    return m_ProjectionMatrix;
}
