//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <Engine/Core/Core.hpp>

#include <spdlog/spdlog.h>

LMC_API CFoo *
mem_alloc()
{
	return new CFoo();
}

LMC_API void
mem_free(CFoo *ptr)
{
	delete ptr;
}

void
CFoo::Apply()
{
	spdlog::info("CFoo concept Apple() called");
}
