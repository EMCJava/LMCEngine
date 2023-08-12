//
// Created by loyus on 8/1/2023.
//

#include "PureConceptCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( PureConceptCamera )

void
PureConceptCamera::SetDimensions( int Width, int Height )
{
    m_CameraWidth  = Width;
    m_CameraHeight = Height;

    UpdateProjectionMatrix( );
}

const glm::mat4&
PureConceptCamera::GetProjectionMatrix( ) const
{
    return m_ProjectionMatrix;
}

void
PureConceptCamera::UpdateProjectionMatrix( )
{
    m_ProjectionMatrix = glm::ortho<FloatTy>( -m_CameraWidth / m_Scale / 2 + m_Coordinate.X,
                                              m_CameraWidth / m_Scale / 2 + m_Coordinate.X,
                                              -m_CameraHeight / m_Scale / 2 + m_Coordinate.Y,
                                              m_CameraHeight / m_Scale / 2 + m_Coordinate.Y,
                                              -1, 1 );
}

void
PureConceptCamera::SetScale( FloatTy Scale )
{
    m_Scale = Scale;

    UpdateProjectionMatrix( );
}
