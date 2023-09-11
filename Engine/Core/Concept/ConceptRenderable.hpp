//
// Created by loyus on 7/13/2023.
//

#pragma once

#include "Concept.hpp"

class ConceptRenderable : public Concept
{
    DECLARE_CONCEPT( ConceptRenderable, Concept )

public:
    /*
     *
     * Try to get camera to default camera
     *
     * */
    ConceptRenderable( );

    virtual void
    Render( ) = 0;

    void
    SetActiveCamera( class PureConceptCamera* ActiveCamera );

    class PureConceptCamera*
    GetActiveCamera( ) { return m_ActiveCamera; }

protected:
    class PureConceptCamera* m_ActiveCamera = nullptr;
};
