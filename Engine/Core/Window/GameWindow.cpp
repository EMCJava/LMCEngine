//
// Created by loyus on 7/2/2023.
//

#include "GameWindow.hpp"

#include <Engine/Engine.hpp>

#include <glad/gl.h>

#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>

#include RootConceptIncludePath

void
GameWindow::Update( )
{
    if ( m_RootConcept != nullptr )
    {
        MakeContextCurrent( );

        auto* RootConcept = GetConceptPtr( );

        if ( RootConcept->CanCastV( Concept::TypeID ) )
        {
            ( (Concept*) RootConcept )->GetConcepts( m_ConceptRenderables );

            if ( m_ConceptRenderables.NotEmpty( ) )
            {
                const auto MainWindowViewPortDimensions = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );

                // we update the camera to new viewport size here
                if ( MainWindowViewPortDimensions != m_ViewportDimension )
                {
                    m_ViewportDimension        = MainWindowViewPortDimensions;
                    m_ViewportLogicalDimension = Engine::GetEngine( )->GetLogicalMainWindowViewPortDimensions( );

                    spdlog::info( "Viewport dimensions changed to {}x{}", m_ViewportDimension.first, m_ViewportDimension.second );
                    spdlog::info( "Camera dimensions changed to {}x{}", m_ViewportLogicalDimension.first, m_ViewportLogicalDimension.second );

                    ( (Concept*) RootConcept )->GetConcepts( m_ConceptCameras );
                    m_ConceptCameras.ForEach( [ this ]( std::shared_ptr<PureConceptCamera>& item ) {
                        item->SetDimensions( m_ViewportLogicalDimension.first, m_ViewportLogicalDimension.second );
                    } );
                }

                /*
                 *
                 * Render every registered ConceptRenderable
                 *
                 * */
                m_GLContext->Viewport( 0, 0, m_ViewportDimension.first, m_ViewportDimension.second );
                m_GLContext->Enable( GL_BLEND );
                m_GLContext->BlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                m_GLContext->ClearColor( 0.F, 0.F, 0.F, 1.0f );
                m_GLContext->Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                m_ConceptRenderables.ForEach( []( std::shared_ptr<ConceptRenderable>& item ) {
                    item->Render( );
                } );
            }
        }
    }
}

void
GameWindow::SetRootConcept( class RootConceptTy* RootConcept )
{
    m_RootConcept = RootConcept;

    if ( RootConcept != nullptr )
    {
#ifdef LMC_EDITOR
        m_RootConceptFakeShared = std::shared_ptr<PureConcept>( RootConcept->As<PureConcept>( ), []( PureConcept* ) {} );
#else
        m_RootConceptFakeShared = std::shared_ptr<PureConcept>( RootConcept, []( PureConcept* ) {} );
#endif
    }

    m_ConceptCameras.Clear( );
    m_ConceptRenderables.Clear( );

    m_ViewportLogicalDimension = m_ViewportPhysicalDimension = m_ViewportDimension = { };
}

class PureConcept*
GameWindow::GetConceptPtr( )
{
    if ( m_RootConcept == nullptr ) return nullptr;

#ifdef LMC_EDITOR
    return m_RootConcept->As<PureConcept>( );
#else
    return m_RootConcept;
#endif
}

std::shared_ptr<PureConcept>
GameWindow::GetConceptFakeSharedPtr( )
{
    return std::static_pointer_cast<PureConcept>( m_RootConceptFakeShared );
}
