// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect.h"

#include <Engine/Core/Concept/ConceptCoreRuntime.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

void
ToImGuiWidget( const char*, SaEffect* Value )
{
    SIMPLE_LIST_DEFAULT_IMGUI_TYPE( IsModifier, Iteration, ElementCount );
}

SaEffect::SaEffect( )
{
}

SaEffect::~SaEffect( )
{
}
