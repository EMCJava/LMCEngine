//
// Created by loyus on 7/10/2023.
//

#include "OrientationCoordinate.hpp"

auto
OrientationCoordinate::GetCoordinate( ) const -> const Coordinate&
{
    return m_Coordinate;
}

auto
OrientationCoordinate::GetCoordinate( ) -> Coordinate&
{
    return m_Coordinate;
}
