//
// Created by samsa on 10/17/2023.
//

#include "PureConceptOrthoCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( PureConceptOrthoCamera )

void
PureConceptOrthoCamera::SetScale( FloatTy Scale )
{
    m_Scale = Scale;

    UpdateProjectionMatrix( );
}

FloatTy
PureConceptOrthoCamera::GetScale( ) const
{
    return m_Scale;
}

void
PureConceptOrthoCamera::ScreenCoordToUICoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const
{
    // UI has coordinate 0,0 at the center of the screen
    ScreenCoord.first -= m_CameraWidth / 2.0F;
    ScreenCoord.second = -( ScreenCoord.second - m_CameraHeight / 2.0F );   // Reverse Y-axis

    // Camera Scale
    ScreenCoord.first /= m_Scale;
    ScreenCoord.second /= m_Scale;
}

void
PureConceptOrthoCamera::ScreenCoordToWorldCoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const
{
    // UI has coordinate 0,0 at the center of the screen
    ScreenCoord.first -= m_CameraWidth / 2.0F;
    ScreenCoord.second = -( ScreenCoord.second - m_CameraHeight / 2.0F );   // Reverse Y-axis

    // Reverse camera offset
    ScreenCoord.first += m_Coordinate.x;
    ScreenCoord.second += m_Coordinate.y;

    // Camera Scale
    ScreenCoord.first /= m_Scale;
    ScreenCoord.second /= m_Scale;
}


void
PureConceptOrthoCamera::SetCoordinate( FloatTy X, FloatTy Y, FloatTy )
{
    m_Coordinate.x = X;
    m_Coordinate.y = Y;
}

const glm::vec3&
PureConceptOrthoCamera::GetCoordinate( )
{
    return m_Coordinate;
}

void
PureConceptOrthoCamera::UpdateProjectionMatrix( )
{
    m_ProjectionMatrix = glm::ortho<FloatTy>( -m_CameraWidth / m_Scale / 2 + m_Coordinate.x,
                                              m_CameraWidth / m_Scale / 2 + m_Coordinate.x,
                                              -m_CameraHeight / m_Scale / 2 + m_Coordinate.y,
                                              m_CameraHeight / m_Scale / 2 + m_Coordinate.y,
                                              -1, 1 );
}
