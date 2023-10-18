//
// Created by samsa on 10/12/2023.
//

#include "Canvas.hpp"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>

#include <ranges>
#include <utility>

DEFINE_CONCEPT_DS( Canvas )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( Canvas, ConceptRenderable, m_CanvasCamera )

Canvas::Canvas( )
{
    // Render any renderable in the canvas
    SetSearchThrough( );
}

void
Canvas::Render( )
{
    auto* CameraPtr = m_CanvasCamera.get( );
    if ( CameraPtr == nullptr ) CameraPtr = m_ActiveCamera;
    REQUIRED_IF( CameraPtr != nullptr )
    {
        CameraPtr->PushToCameraStack( );
        std::vector<std::shared_ptr<ConceptRenderable>> AnySubRenderables;
        GetConcepts( AnySubRenderables );
        for ( const auto& R : AnySubRenderables )
        {
            R->SetActiveCamera( CameraPtr );
        }
        CameraPtr->PopFromCameraStack( );
    }
}

void
Canvas::SetCanvasCamera( const std::shared_ptr<PureConceptCamera>& Camera )
{
    m_CanvasCamera = Camera;
}
