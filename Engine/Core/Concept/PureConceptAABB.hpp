//
// Created by samsa on 8/31/2023.
//

#pragma once

#include "PureConcept.hpp"

class PureConceptAABB : public PureConcept
{
    DECLARE_CONCEPT( PureConceptAABB, PureConcept )

    virtual bool
    HitTest( const std::pair<FloatTy, FloatTy>& HitPoint ) = 0;
};
