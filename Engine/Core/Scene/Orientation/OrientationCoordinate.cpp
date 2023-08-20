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

const OrientationCoordinate::Coordinate&
OrientationCoordinate::GetOrigin( ) const
{
    return m_Origin;
}

const OrientationCoordinate::Coordinate&
OrientationCoordinate::AlterOrigin( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Origin.X += X;
    m_Origin.Y += Y;
    m_Origin.Z += Z;

    UpdateTranslationMatrix( );

    return m_Origin;
}

const OrientationCoordinate::Coordinate&
OrientationCoordinate::SetOrigin( FloatTy X, FloatTy Y, FloatTy Z )
{
    m_Origin.X = X;
    m_Origin.Y = Y;
    m_Origin.Z = Z;

    UpdateTranslationMatrix( );

    return m_Origin;
}

glm::mat4&
OrientationCoordinate::GetTranslationMatrix( )
{
    return m_TranslationMatrix;
}

void
OrientationCoordinate::UpdateTranslationMatrix( )
{
    m_TranslationMatrix = glm::translate( glm::mat4( 1 ), glm::vec3( m_Coordinate.X, m_Coordinate.Y, m_Coordinate.Z ) - glm::vec3( m_Origin.X, m_Origin.Y, m_Origin.Z ) );
}

const OrientationCoordinate::Coordinate&
OrientationCoordinate::SetCoordinate( OrientationCoordinate::Coordinate Coor )
{
    m_Coordinate = Coor;

    UpdateTranslationMatrix( );

    return m_Coordinate;
}
