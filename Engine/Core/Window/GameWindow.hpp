//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Concept/ConceptSetCache.hpp>

class GameWindow : public Window
{
public:
    using Window::Window;

    void
    UpdateCamerasDimension( );

    void
    Update( ) override;

    virtual void
    SetRootConcept( class RootConceptTy* RootConcept );

protected:
    /*
     *
     * Main viewport setup
     *
     * */
    std::pair<int, int> m_ViewportLogicalDimension { };
    std::pair<int, int> m_ViewportPhysicalDimension { };
    std::pair<int, int> m_ViewportDimension { };

    class RootConceptTy*         m_RootConcept = nullptr;
    std::shared_ptr<PureConcept> m_RootConceptFakeShared { };

    class PureConcept*
    GetConceptPtr( );

    std::shared_ptr<PureConcept>
    GetConceptFakeSharedPtr( );

    ConceptSetCacheShared<ConceptRenderable> m_ConceptRenderables { };
    ConceptSetCacheShared<PureConceptCamera> m_ConceptCameras { };
};
