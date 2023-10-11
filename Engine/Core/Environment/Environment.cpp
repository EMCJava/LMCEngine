//
// Created by loyus on 7/2/2023.
//

#include "Environment.hpp"

#include <Engine/Core/Window/Window.hpp>
#include <Engine/Core/Network/NetController.hpp>

void
InitializeEnvironment( )
{
    InitializeWindowEnvironment( );
    NetController::NetGlobalInitialize( );
}

void
ShutdownEnvironment( )
{
    ShutdownWindowEnvironment( );
    NetController::NetGlobalShutdown( );
}