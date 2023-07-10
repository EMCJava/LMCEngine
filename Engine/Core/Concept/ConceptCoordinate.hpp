//
// Created by loyus on 7/10/2023.
//

#pragma once

#include "Concept.hpp"

// #include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

class ConceptCoordinate : public Concept
{
	DECLARE_CONCEPT(ConceptCoordinate, Concept)
public:
	struct Coordinate {
		FloatTy X = 0, Y = 0, Z = 0;
	};

	const Coordinate &
	GetCoordinate() const;

	Coordinate &
	GetCoordinate();

private:
	Coordinate m_Coordinate{};
};

template<>
struct fmt::formatter<ConceptCoordinate::Coordinate> : fmt::formatter<std::string> {
	static auto
	format(const ConceptCoordinate::Coordinate &C, format_context &ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "[Coordinate X={} Y={} Z={}]", C.X, C.Y, C.Z);
	}
};