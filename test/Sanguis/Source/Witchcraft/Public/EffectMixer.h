// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Effect.h"

enum class SaCombineMethod
{
	CombineMethodNone  = 0x0,
	CombineMethodRight = 0x1,
	CombineMethodLeft  = 0x2,

	CombineMethodMix = CombineMethodRight | CombineMethodLeft,
};

ENUM_CLASS_FLAGS(SaCombineMethod)

SaEffect Combine(const SaCombineMethod Method, const SaEffect& A, const SaEffect& B);