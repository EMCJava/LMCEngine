//
// Created by loyus on 8/1/2023.
//

#include "PureConceptCamera.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>

#include <glm/gtc/matrix_transform.hpp>

DEFINE_CONCEPT_DS( PureConceptCamera )
DEFINE_SIMPLE_IMGUI_TYPE( PureConceptCamera, m_CameraWidth, m_CameraHeight, m_Scale )

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
    m_ProjectionMatrix = glm::ortho<FloatTy>( -m_CameraWidth / m_Scale / 2 + m_Coordinate.x,
                                              m_CameraWidth / m_Scale / 2 + m_Coordinate.x,
                                              -m_CameraHeight / m_Scale / 2 + m_Coordinate.y,
                                              m_CameraHeight / m_Scale / 2 + m_Coordinate.y,
                                              -1, 1 );

    m_ProjectionMatrixNonOffset = glm::ortho<FloatTy>( -m_CameraWidth / m_Scale / 2,
                                                       m_CameraWidth / m_Scale / 2,
                                                       -m_CameraHeight / m_Scale / 2,
                                                       m_CameraHeight / m_Scale / 2,
                                                       -1, 1 );
}

void
PureConceptCamera::SetScale( FloatTy Scale )
{
    m_Scale = Scale;

    UpdateProjectionMatrix( );
}

const glm::mat4&
PureConceptCamera::GetProjectionMatrixNonOffset( ) const
{
    return m_ProjectionMatrixNonOffset;
}

std::pair<FloatTy, FloatTy>
PureConceptCamera::GetDimensions( ) const
{
    return { m_CameraWidth, m_CameraHeight };
}

FloatTy
PureConceptCamera::GetScale( ) const
{
    return m_Scale;
}

void
PureConceptCamera::ScreenCoordToUICoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const
{
    // UI has coordinate 0,0 at the center of the screen
    ScreenCoord.first -= m_CameraWidth / 2.0F;
    ScreenCoord.second = -( ScreenCoord.second - m_CameraHeight / 2.0F );   // Reverse Y-axis

    // Camera Scale
    ScreenCoord.first /= m_Scale;
    ScreenCoord.second /= m_Scale;
}

void
PureConceptCamera::ScreenCoordToWorldCoord( std::pair<FloatTy, FloatTy>& ScreenCoord ) const
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
PureConceptCamera::RegisterAsDefaultCamera( )
{
    Engine::GetEngine( )->GetGlobalResourcePool( )->TryPush( "DefaultCamera", shared_from_this( ) );
}

void
PureConceptCamera::SetCoordinate( FloatTy X, FloatTy Y, FloatTy )
{
    m_Coordinate.x = X;
    m_Coordinate.y = Y;
}

const glm::vec3&
PureConceptCamera::GetCoordinate( )
{
    return m_Coordinate;
}
