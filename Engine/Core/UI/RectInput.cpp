//
// Created by EMCJava on 4/1/2024.
//

#include "RectInput.hpp"

#include <Engine/Core/Input/UserInput.hpp>

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

#include <GLFW/glfw3.h>

DEFINE_CONCEPT_DS( RectInput )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RectInput, RectButton, m_Selected )

RectInput::RectInput( int Width, int Height )
    : RectButton( Width, Height )
{
    // SetCallback( );
}

void
RectInput::Apply( )
{
    RectButton::Apply( );

    if ( m_MousePressThisFrame ) m_Selected = m_ActivatedThisFrame;

    m_PressReactTimeLeft = 0;
    if ( m_Selected )
    {
        m_PressReactTimeLeft = 1;

        const auto& NewCharPresses = Engine::GetEngine( )->GetUserInputHandle( )->GetNewCharPresses( );

        if ( !NewCharPresses.empty( ) )
        {
            for ( const auto& CharPress : NewCharPresses )
                m_ButtonTextStr += (char) CharPress;
            SetText( m_ButtonTextStr );
        }

        const auto DeleteCount = std::ranges::count( Engine::GetEngine( )->GetUserInputHandle( )->GetNewKeyPresses( ), GLFW_KEY_BACKSPACE );
        if ( DeleteCount ) SetText( m_ButtonTextStr.substr( 0, m_ButtonTextStr.size( ) - DeleteCount ) );
    }
}
