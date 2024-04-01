//
// Created by EMCJava on 4/1/2024.
//

#include "RectInput.hpp"

#include <Engine/Core/Input/UserInput.hpp>

#include <GLFW/glfw3.h>

DEFINE_CONCEPT_DS( RectInput )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( RectInput, RectButton )

RectInput::RectInput( int Width, int Height )
    : RectButton( Width, Height )
{
    // SetCallback( );
}

void
RectInput::Apply( )
{
    RectButton::Apply( );

    const auto& NewKeyPresses = Engine::GetEngine( )->GetUserInputHandle( )->GetNewKeyPresses( );
    for ( const auto& KeyPress : NewKeyPresses )
    {
        spdlog::info( "New Key Press: {}", glfwGetKeyName( KeyPress, 0 ) );
    }
}
