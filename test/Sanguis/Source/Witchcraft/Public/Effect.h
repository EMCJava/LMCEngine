// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "BasicElement/AllElement.h"

/**
 * 
 */
class SANGUIS_API SaEffect
{
public:
	SaEffect();
	~SaEffect();

	// Only can modify other SaEffect, not used directly
	bool     IsModifier = true;
	uint32_t Iteration  = 0;

	TStaticArray<uint8, NumOfElement> ElementCount;
};
