﻿#pragma once

#include "Effect.h"

enum class SaCombineMethod
{
	CombineMethodNone  = 0x0,
	CombineMethodRight = 0x1,
	CombineMethodLeft  = 0x2,

	CombineMethodMix = CombineMethodRight | CombineMethodLeft,
};

SaEffect Combine(SaCombineMethod Method, const SaEffect& A, const SaEffect& B);