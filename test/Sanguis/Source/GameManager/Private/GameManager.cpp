//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
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
#include <sstream>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, PureConcept, m_Effect, TestInvokable )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );
    auto DefaultShader           = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" );
    auto DefaultShaderInstancing = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShaderInstancing" );

    {
        m_Camera = AddConcept<PureConceptCamera>( );
        m_Camera->RegisterAsDefaultCamera( );

        m_Camera->SetCoordinate( 1920 / 2, 1080 / 2 );
    }

    AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/Inv.png" ) );

    {
        auto Sp = AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/wand.png" ) );
        Sp->SetScale( 0.7, 0.7 );
        Sp->SetCoordinate( 800, 181.6 );
    }

    {
        m_PAR = std::make_unique<ParticleAttributesRandomizer>( );
        m_PAR->SetLinearScale( { 0.01, 0.01, 0.01 }, { 0.1, 0.1, 0.1 } );
        m_PAR->SetVelocity( { -20, -20, 0 }, { 20, 20, 0 } );
        m_PAR->SetAngularVelocity( -31.415F, 31.415F );
        m_PAR->SetStartLinearColor( glm::vec4( 1, 0, 0, 0.9 ), glm::vec4( 1, 0.2, 0, 1 ) );
        m_PAR->SetEndLinearColor( glm::vec4( 1, 0.6, 0.4, 0 ), glm::vec4( 1, 0.8, 0.6, 0 ) );
        m_PAR->SetLifetime( 100, 500 );

        m_ParticlePools.push_back( AddConcept<ParticlePool>( ) );
        m_ParticlePools.back( )->SetSprite( std::make_shared<SpriteSquareTexture>( DefaultShaderInstancing, std::make_shared<PureConceptImage>( "Assets/Texture/Particle/star.png" ) ) );
        m_ParticlePools.push_back( AddConcept<ParticlePool>( ) );
        m_ParticlePools.back( )->SetSprite( std::make_shared<SpriteSquareTexture>( DefaultShaderInstancing, std::make_shared<PureConceptImage>( "Assets/Texture/Particle/ring.png" ) ) );
    }

    SaBaseBoard BB;

    {
        std::ifstream     BoardTemp( "Assets/Boards/StarterBoard.yaml" );
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
    if ( Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( ).isDown )
    {
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_Camera->ScreenCoordToUICoord( HitPoint );

        auto& PP = m_ParticlePools[ 0 ];

        auto* Pa = &PP->AddParticle( );
        m_PAR->Apply( *Pa );
        Pa->GetOrientation( ).SetOrigin( 512 / 2, 512 / 2 );
        Pa->GetOrientation( ).SetCoordinate( HitPoint.first, HitPoint.second );
    }

    if ( Engine::GetEngine( )->GetUserInputHandle( )->GetSecondaryKey( ).isDown )
    {
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_Camera->ScreenCoordToUICoord( HitPoint );

        auto& PP = m_ParticlePools[ 1 ];

        auto* Pa = &PP->AddParticle( );
        m_PAR->Apply( *Pa );
        Pa->GetOrientation( ).SetOrigin( 512 / 2, 512 / 2 );
        Pa->GetOrientation( ).SetCoordinate( HitPoint.first, HitPoint.second );
    }
}

void
GameManager::TestInvokable( )
{
    auto& PP = m_ParticlePools[ rand( ) % m_ParticlePools.size( ) ];

    auto* Pa = &PP->AddParticle( );
    m_PAR->Apply( *Pa );
    Pa->GetOrientation( ).SetOrigin( 512 / 2, 512 / 2 );
    Pa->GetOrientation( ).SetCoordinate( 0, 0 );
}
