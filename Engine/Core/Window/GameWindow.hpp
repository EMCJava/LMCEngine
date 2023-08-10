//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Concept/ConceptSetFetchCache.hpp>

class GameWindow : public Window
{
public:
    using Window::Window;

    void
    Update( ) override;

    void
    SetRootConcept( class RootConceptTy* RootConcept );

protected:
    /*
     *
     * Main viewport setup
     *
     * */
    std::pair<int, int> m_MainViewPortDimension { };

    class RootConceptTy* m_RootConcept = nullptr;

    class Concept* GetConceptPtr( );

    ConceptSetFetchCache<ConceptRenderable> m_ConceptRenderables { };
    ConceptSetFetchCache<PureConceptCamera> m_ConceptCameras { };
};
