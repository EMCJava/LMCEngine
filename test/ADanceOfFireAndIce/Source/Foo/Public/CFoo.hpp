//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>

class CFoo : public ConceptApplicable
{
	DECLARE_CONCEPT(CFoo, ConceptApplicable)

public:
	CFoo();

	void
	Apply() override;
};