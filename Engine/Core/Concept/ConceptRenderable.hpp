//
// Created by loyus on 7/13/2023.
//

#pragma once

#include "Concept.hpp"

class ConceptRenderable : public Concept
{
    DECLARE_CONCEPT( ConceptRenderable, Concept )

public:
    virtual void
    Render( ) = 0;
};
