//
// Created by loyus on 7/4/2023.
//

#include "Concept.hpp"

#include <random>

DEFINE_CONCEPT(Concept)

Concept::Concept()
{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	m_SubConceptsStateHash.SetSeed(std::array{dis(gen), dis(gen)});
}
