//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <Engine/Core/Core.hpp>

#include <spdlog/spdlog.h>

LMC_API void *
mem_alloc()
{
	return new CFoo();
}

LMC_API void
mem_free(void *ptr)
{
	delete (CFoo *)ptr;
}

CFoo::CFoo()
{
	spdlog::info("CFoo concept constructor called");
}

CFoo::~CFoo()
{
	spdlog::info("CFoo concept destructor called");
}

void
CFoo::Apply()
{
	spdlog::info("CFoo concept Apply() called");
}
