//
// Created by loyus on 7/10/2023.
//

#include "OrientationCoordinate.hpp"

#include <glm/gtc/matrix_transform.hpp>

auto
OrientationCoordinate::GetCoordinate( ) const -> const Coordinate&
{
    return m_Coordinate;
}

const OrientationCoordinate::Coordinate&
OrientationCoordinate::AlterCoordinate( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Coordinate.X += X;
    m_Coordinate.Y += Y;
    m_Coordinate.Z += Z;

    UpdateTranslationMatrix( );

    return m_Coordinate;
}

const OrientationCoordinate::Coordinate&
OrientationCoordinate::SetCoordinate( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Coordinate.X = X;
    m_Coordinate.Y = Y;
    m_Coordinate.Z = Z;

    UpdateTranslationMatrix( );

    return m_Coordinate;
}

glm::mat4&
OrientationCoordinate::GetTranslationMatrix( )
{
    return m_TranslationMatrix;
}

void
OrientationCoordinate::UpdateTranslationMatrix( )
{
    m_TranslationMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_Coordinate.X, m_Coordinate.Y, m_Coordinate.Z ) );
}
