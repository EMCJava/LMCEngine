// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect.h"

/**
 *
 */
class SANGUIS_API SaControlNode
{
public:
	virtual ~SaControlNode() = default;
	SaControlNode();

	SaEffect GetEffect()
	{
		SaEffect Result;
		GetEffect(Result);
		return Result;
	}

	virtual void GetEffect(SaEffect& Result) = 0;
};