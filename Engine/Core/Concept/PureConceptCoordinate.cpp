//
// Created by loyus on 7/10/2023.
//

#include "PureConceptCoordinate.hpp"

auto
PureConceptCoordinate::GetCoordinate() const -> const Coordinate &
{
	return m_Coordinate;
}

auto
PureConceptCoordinate::GetCoordinate() -> Coordinate &
{
	return m_Coordinate;
}
