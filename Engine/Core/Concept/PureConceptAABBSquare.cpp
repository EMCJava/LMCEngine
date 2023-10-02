//
// Created by samsa on 8/31/2023.
//

#include "PureConceptAABBSquare.hpp"
#include "ConceptCoreToImGuiImpl.hpp"

DEFINE_CONCEPT_DS( PureConceptAABBSquare )
DEFINE_SIMPLE_IMGUI_TYPE( PureConceptAABBSquare, X, Y, W, H )

PureConceptAABBSquare::PureConceptAABBSquare( FloatTy X, FloatTy Y, FloatTy W, FloatTy H )
{
    this->X = X;
    this->Y = Y;
    this->W = W;
    this->H = H;
}

bool
PureConceptAABBSquare::HitTest( const std::pair<FloatTy, FloatTy>& HitPoint )
{
    // if ( !m_Enabled ) return false;

    return HitPoint.first >= this->X && HitPoint.first <= this->X + this->W
        && HitPoint.second >= this->Y && HitPoint.second <= this->Y + this->H;
}

void
PureConceptAABBSquare::SetCoordinate( FloatTy X, FloatTy Y )
{
    this->X = X;
    this->Y = Y;
}

void
PureConceptAABBSquare::SetSetDimension( FloatTy W, FloatTy H )
{
    this->W = W;
    this->H = H;
}
