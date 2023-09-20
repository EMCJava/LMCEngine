//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "BaseBoard.h"

#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <fstream>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, PureConcept, m_Effect )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    m_Effect = std::make_unique<SaEffect>( );

    SaBaseBoard BB;

    {
        std::ifstream     BoardTemp( "Assets/Boards/StarterBoard.json" );
        std::stringstream buffer;
        buffer << BoardTemp.rdbuf( );

        BB.Serialize( buffer.str( ) );
    }

    BB.GetEffect( *m_Effect );

    spdlog::info( "Effect.Iteration : {}", m_Effect->Iteration );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
}