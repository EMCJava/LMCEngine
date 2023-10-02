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
#include <Engine/Core/Concept/PureConceptAABBSquare.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, PureConcept, m_Effect, TestInvokable )

namespace
{
inline std::pair<FloatTy, FloatTy>
operator-( const std::pair<FloatTy, FloatTy>& lhs, const std::pair<FloatTy, FloatTy>& rhs )
{
    return std::make_pair( lhs.first - rhs.first, lhs.second - rhs.second );
}
}   // namespace

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

    const glm::vec3 EditingAreaCoord = { 600, 30, 0 };
    const FloatTy   EditorAreaScale  = 0.7F;
    glm::vec3       BoardDimensions { };

    {
        auto Sp         = AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/Boards/StarterBoard.png" ) );
        BoardDimensions = { Sp->GetSpriteDimensions( ).first, Sp->GetSpriteDimensions( ).second, 0 };
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( EditorAreaScale, EditorAreaScale );
        Sp->SetCoordinate( EditingAreaCoord + BoardDimensions / 2.F );
    }

    {
        auto Sp = AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/wand.png" ) );
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( EditorAreaScale, EditorAreaScale );
        Sp->SetCoordinate( EditingAreaCoord + BoardDimensions / 2.F );
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

        auto BBS = std::make_shared<SaBaseBoard>( );
        BB.Serialize( buffer.str( ) );
        BBS->Serialize( buffer.str( ) );

        {
            auto SlotPAR = std::make_unique<ParticleAttributesRandomizer>( );
            SlotPAR->SetAngularVelocity( -10, 10 );

            const auto SlotBaseScale = glm::vec3( 0.45F * EditorAreaScale, 0.45F * EditorAreaScale, 1 );
            SlotPAR->SetLinearScale( SlotBaseScale - glm::vec3( 0.05F ), SlotBaseScale + glm::vec3( 0.05F ) );

            const auto SlotParticleCount         = 10;
            const auto ControlNodeSpriteLocation = BB.GetControlNodeSpriteLocation( );
            for ( const auto& [ ID, Location ] : ControlNodeSpriteLocation )
            {
                auto ScaledLocation = std::pair<FloatTy, FloatTy> { 0.5F, 0.5F } - Location;
                ScaledLocation      = { ScaledLocation.first * EditorAreaScale, ScaledLocation.second * EditorAreaScale };
                ScaledLocation      = std::pair<FloatTy, FloatTy> { 0.5F, 0.5F } - ScaledLocation;

                auto& PP = m_ParticlePools[ 1 ];

                const auto SlotCoordinate = EditingAreaCoord + BoardDimensions * glm::vec3( ScaledLocation.first, ScaledLocation.second, 1 );
                for ( int i = 0; i < SlotParticleCount; ++i )
                {
                    auto* Pa = &PP->AddParticle( );
                    SlotPAR->Apply( *Pa );

                    Pa->SetLifeTime( std::numeric_limits<FloatTy>::infinity( ) );
                    Pa->GetColor( ) = glm::vec4( 1 );
                    Pa->GetOrientation( ).SetOrigin( 512 / 2, 512 / 2 );
                    Pa->GetOrientation( ).SetCoordinate( SlotCoordinate );
                }

                // Hit box
                const auto ActualSize    = SlotBaseScale * 512.F;
                m_BaseSlotHitBoxes[ ID ] = AddConcept<PureConceptAABBSquare>( SlotCoordinate.x - ActualSize.x / 2, SlotCoordinate.y - ActualSize.y / 2, ActualSize.x, ActualSize.y );
                spdlog::info( "HitBox: {}, {}", SlotCoordinate.x - 512 / 2, SlotCoordinate.y - 512 / 2 );
            }
        }

        BB.SetSlot( "A", std::move( BBS ) );
        BB.SetSlot( "B", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( "C", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( "D", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        BB.SetSlot( "E", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
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
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_Camera->ScreenCoordToUICoord( HitPoint );

        for ( const auto& [ ID, HitBox ] : m_BaseSlotHitBoxes )
        {
            if ( HitBox->HitTest( HitPoint ) )
            {
                spdlog::info( "BaseSlot HitTest : {}", ID );
            }
        }
    }

    if ( Engine::GetEngine( )->GetUserInputHandle( )->GetSecondaryKey( ).isDown )
    {
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_Camera->ScreenCoordToUICoord( HitPoint );

        auto& PP = m_ParticlePools[ rand( ) % 2 ];

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
