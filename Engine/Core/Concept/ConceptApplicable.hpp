//
// Created by loyus on 7/10/2023.
//

#pragma once

#include "ConceptList.hpp"

class ConceptApplicable : public ConceptList
{
    DECLARE_CONCEPT( ConceptApplicable, ConceptList )

public:
    /*
     *
     * Apply the concept to environment, serve the same function as update
     *
     * */
    virtual void
    Apply( ) = 0;
};
