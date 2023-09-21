//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticlePool.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticleAttributesRandomizer.hpp>
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

    AddConcept<PureConceptCamera>( )->RegisterAsDefaultCamera( );

    {
        auto Sp = std::make_shared<SpriteSquareTexture>( 512, 512 );
        Sp->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" ) );
        Sp->SetTexturePath( "Assets/Texture/Particle/ring.png" );
        Sp->SetupSprite( );

        m_ParticlePool = AddConcept<ParticlePool>( );
        m_ParticlePool->SetSprite( Sp );

        ParticleAttributesRandomizer PAR;
        PAR.SetVelocity( { -20, -20, 0 }, { 20, 20, 0 } );
        PAR.SetAngularVelocity( -31.415F, 31.415F );

        const auto AddParticle = [ this, &PAR ]( ) {
            auto* Pa = &m_ParticlePool->AddParticle( );
            PAR.Apply( *Pa );
            Pa->GetOrientation( ).SetRotationCenter( 512 / 2, 512 / 2 );
            Pa->GetOrientation( ).SetOrigin( 512 / 2, 512 / 2 );
        };

        for ( int i = 0; i < 2000; ++i )
            AddParticle( );
    }

    SaBaseBoard BB;

    {
        std::ifstream     BoardTemp( "Assets/Boards/StarterBoard.json" );
        std::stringstream buffer;
        buffer << BoardTemp.rdbuf( );

        BB.Serialize( buffer.str( ) );

        auto BBS = std::make_shared<SaBaseBoard>( );
        BBS->Serialize( buffer.str( ) );

        BB.SetSlot( 0, std::move( BBS ) );
        BB.SetSlot( 1, std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( 2, std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( 3, std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( 4, std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
    }

    m_Effect = std::make_unique<SaEffect>( );
    BB.GetEffect( *m_Effect );

    spdlog::info( "Effect.Iteration : {}", m_Effect->Iteration );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    if ( Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( ).isPressed )
    {
    }
}