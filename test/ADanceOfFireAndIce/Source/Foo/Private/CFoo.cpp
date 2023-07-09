//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <spdlog/spdlog.h>

DEFINE_CONCEPT(CFoo, Concept)

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
