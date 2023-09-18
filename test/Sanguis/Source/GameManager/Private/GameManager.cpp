//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "BaseBoard.h"

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_NECESSARY_IMGUI_TYPE( GameManager )
void
GameManager::ToImGuiWidgetInternal( const char*, GameManager*, bool )
{ }

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    SaBaseBoard BB;
    BB.AddDemoData( );

    SaEffect Effect;
    BB.GetEffect( Effect );

    spdlog::info( "Effect.Iteration : {}", Effect.Iteration );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
}