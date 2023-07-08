//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/Concept.hpp>

class CFoo : public Concept
{
	DECLARE_CONCEPT(Concept)

public:
	CFoo();
	~CFoo() override;

	void
	Apply() override;
};

DEFINE_CONCEPT(CFoo, Concept)