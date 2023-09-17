//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_NECESSARY_IMGUI_TYPE( GameManager )
void
GameManager::ToImGuiWidgetInternal( const char*, GameManager*, bool )
{ }

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
}