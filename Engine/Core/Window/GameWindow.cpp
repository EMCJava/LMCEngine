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

        m_ConceptRenderables.Clear( );
        if ( RootConcept->CanCastV( Concept::TypeID ) )
        {
            ( (Concept*) RootConcept )->GetConcepts<ConceptRenderable>( m_ConceptRenderables );

            if ( m_ConceptRenderables.NotEmpty( ) )
            {
                const auto MainWindowViewPortDimensions = Engine::GetEngine( )->GetMainWindowViewPortDimensions( );

                // we rescale the framebuffer to the actual window size here and reset the glViewport
                if ( MainWindowViewPortDimensions != m_MainViewPortDimension )
                {
                    m_MainViewPortDimension = MainWindowViewPortDimensions;

                    ( (Concept*) RootConcept )->GetConcepts<PureConceptCamera>( m_ConceptCameras );
                    m_ConceptCameras.ForEach( [ this ]( PureConceptCamera* item ) {
                        item->SetDimensions( m_MainViewPortDimension.first, m_MainViewPortDimension.second );
                    } );
                }

                /*
                 *
                 * Render every registered ConceptRenderable
                 *
                 * */
                m_GLContext->Viewport( 0, 0, m_MainViewPortDimension.first, m_MainViewPortDimension.second );
                m_GLContext->Enable( GL_BLEND );
                m_GLContext->BlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                m_GLContext->ClearColor( 0.85f, 0.83f, 0.84f, 1.0f );
                m_GLContext->Clear( GL_COLOR_BUFFER_BIT );

                m_ConceptRenderables.ForEach( []( ConceptRenderable* item ) {
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
    m_ConceptRenderables.Clear( );
}

class PureConcept*
GameWindow::GetConceptPtr( )
{
    if ( m_RootConcept == nullptr ) return nullptr;

#ifdef HOT_RELOAD
    return m_RootConcept->As<PureConcept>( );
#else
    return m_RootConcept;
#endif
}
