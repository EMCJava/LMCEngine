//
// Created by loyus on 7/10/2023.
//

#pragma once

#include "Concept.hpp"

class ConceptApplicable : public Concept
{
	DECLARE_CONCEPT(ConceptUpdatable, Concept)

public:
	/*
	 *
	 * Apply the concept to environment, serve the same function as update
	 *
	 * */
	virtual void
	Apply() = 0;
};
