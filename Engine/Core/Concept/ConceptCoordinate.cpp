//
// Created by loyus on 7/10/2023.
//

#include "ConceptCoordinate.hpp"

#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT(ConceptCoordinate, Concept)

auto
ConceptCoordinate::GetCoordinate() const -> const Coordinate &
{
	return m_Coordinate;
}

ConceptCoordinate::Coordinate &
ConceptCoordinate::GetCoordinate()
{
	return m_Coordinate;
}
