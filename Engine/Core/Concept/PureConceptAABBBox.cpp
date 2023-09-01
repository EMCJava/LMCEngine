//
// Created by samsa on 8/31/2023.
//

#include "PureConceptAABBBox.hpp"
#include "ConceptCoreToImGuiImpl.hpp"

DEFINE_CONCEPT_DS( PureConceptAABBBox )
DEFINE_SIMPLE_IMGUI_TYPE( PureConceptAABBBox, X, Y, W, H )

PureConceptAABBBox::PureConceptAABBBox( FloatTy X, FloatTy Y, FloatTy W, FloatTy H )
{
    this->X = X;
    this->Y = Y;
    this->W = W;
    this->H = H;
}

bool
PureConceptAABBBox::HitTest( const std::pair<FloatTy, FloatTy>& HitPoint )
{
    return HitPoint.first >= this->X && HitPoint.first <= this->X + this->W
        && HitPoint.second >= this->Y && HitPoint.second <= this->Y + this->H;
}

void
PureConceptAABBBox::SetCoordinate( FloatTy X, FloatTy Y )
{
    this->X = X;
    this->Y = Y;
}

void
PureConceptAABBBox::SetSetDimension( FloatTy W, FloatTy H )
{
    this->W = W;
    this->H = H;
}
