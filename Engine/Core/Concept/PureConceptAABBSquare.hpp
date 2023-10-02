//
// Created by samsa on 8/31/2023.
//

#pragma once

#include "PureConceptAABB.hpp"

class PureConceptAABBSquare : public PureConceptAABB
{
    DECLARE_CONCEPT( PureConceptAABBSquare, PureConceptAABB )
public:
    PureConceptAABBSquare( FloatTy X, FloatTy Y, FloatTy W, FloatTy H );

    virtual bool
    HitTest( const std::pair<FloatTy, FloatTy>& HitPoint );

    void
    SetCoordinate( FloatTy X = 0, FloatTy Y = 0 );

    void
    SetSetDimension( FloatTy W = 0, FloatTy H = 0 );

protected:
    FloatTy X { };
    FloatTy Y { };
    FloatTy W { };
    FloatTy H { };

    ENABLE_IMGUI( PureConceptAABBSquare )
};
