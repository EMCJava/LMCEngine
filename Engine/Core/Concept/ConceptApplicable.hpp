//
// Created by loyus on 7/10/2023.
//

#pragma once

#include "PureConcept.hpp"

class ConceptApplicable : public PureConcept
{
    DECLARE_CONCEPT( ConceptApplicable, PureConcept )

public:
    /*
     *
     * Apply the concept to environment, serve the same function as update
     *
     * */
    virtual void
    Apply( ) = 0;
};
