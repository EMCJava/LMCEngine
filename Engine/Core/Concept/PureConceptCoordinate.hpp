//
// Created by loyus on 7/10/2023.
//

#pragma once

#include "Concept.hpp"

#include <spdlog/fmt/fmt.h>

class PureConceptCoordinate
{
public:
	struct Coordinate {
		FloatTy X = 0, Y = 0, Z = 0;
	};

	const Coordinate &
	GetCoordinate() const;

	Coordinate &
	GetCoordinate();

protected:
	Coordinate m_Coordinate{};
};

template<>
struct fmt::formatter<PureConceptCoordinate::Coordinate> : fmt::formatter<std::string> {
	static auto
	format(const PureConceptCoordinate::Coordinate &C, format_context &ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "[Coordinate X={} Y={} Z={}]", C.X, C.Y, C.Z);
	}
};