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
#include <ranges>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, PureConcept, m_Effect, TestInvokable, m_MenuItems, m_BaseSlots )
DEFINE_SIMPLE_IMGUI_TYPE( SpriteHitBox, ID )

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

    {
        auto Sp           = AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/Boards/StarterBoard.png" ) );
        m_BoardDimensions = { Sp->GetSpriteDimensions( ).first, Sp->GetSpriteDimensions( ).second, 0 };
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( m_EditorAreaScale, m_EditorAreaScale );
        Sp->SetCoordinate( m_EditingAreaCoord + m_BoardDimensions / 2.F );
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

    {
        std::ifstream     BoardTemp( "Assets/Boards/StarterBoard.yaml" );
        std::stringstream buffer;
        buffer << BoardTemp.rdbuf( );

        m_BaseBoard = std::make_unique<SaBaseBoard>( );
        auto BBS    = std::make_shared<SaBaseBoard>( );
        m_BaseBoard->Serialize( buffer.str( ) );
        BBS->Serialize( buffer.str( ) );

        AddSlotHighlightUI( );

        m_BaseBoard->SetSlot( "A", std::move( BBS ) );
        m_BaseBoard->SetSlot( "B", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        m_BaseBoard->SetSlot( "C", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        m_BaseBoard->SetSlot( "D", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
        m_BaseBoard->SetSlot( "E", std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, Fire, 2 } ) );
    }

    {
        auto Sp = AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/wand.png" ) );
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( m_EditorAreaScale, m_EditorAreaScale );
        Sp->SetCoordinate( m_EditingAreaCoord + m_BoardDimensions / 2.F );
    }

    glm::vec3 TopLeftMenu = { 230, 860, 0 };
    glm::vec3 IconSize { 512 * m_EditorAreaScale * m_IconScale };

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 6; ++j )
        {
            auto& Record = m_MenuItems.emplace_back( );

            auto Image        = std::make_shared<PureConceptImage>( rand( ) & 1 ? "Assets/Texture/Elements/fire.png" : "Assets/Texture/Elements/water.png" );
            Record.Sprite     = AddConcept<SpriteSquareTexture>( DefaultShader, std::move( Image ) );
            m_BoardDimensions = { Record.Sprite->GetSpriteDimensions( ).first, Record.Sprite->GetSpriteDimensions( ).second, 0 };
            Record.Sprite->SetCenterAsOrigin( );

            const auto FinalScale = m_EditorAreaScale * m_IconScale;
            Record.Sprite->SetScale( FinalScale, FinalScale );

            const auto MenuCoordinate = TopLeftMenu + IconSize * glm::vec3 { i, -j, 0 };
            Record.Sprite->SetCoordinate( MenuCoordinate );

            const auto ActualSize = FinalScale * 512.F;
            Record.HitBox         = AddConcept<PureConceptAABBSquare>( MenuCoordinate.x - ActualSize / 2, MenuCoordinate.y - ActualSize / 2, ActualSize, ActualSize );

            Record.ID = std::to_string( j * 3 + i );
        }
    }

    m_Effect = std::make_unique<SaEffect>( );
    m_BaseBoard->GetEffect( *m_Effect );

    spdlog::info( "Effect.Iteration : {}", m_Effect->Iteration );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    const auto& PrimaryKey = Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( );
    if ( PrimaryKey.isPressed )
    {
        // First press
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_Camera->ScreenCoordToUICoord( HitPoint );

        spdlog::info( "HitPoint.first : {}, HitPoint.second {}", HitPoint.first, HitPoint.second );

        const auto CheckForPick = [ &HitPoint, this ]( auto& Slot ) {
            if ( Slot.Sprite != nullptr && Slot.HitBox->HitTest( HitPoint ) )
            {
                spdlog::info( "BaseSlot HitTest : {}", Slot.ID );
                m_MenuHoldingSprite   = &Slot;
                m_SpriteStartPosition = Slot.Sprite->GetCoordinate( );
                m_MouseStartPosition  = { HitPoint.first, HitPoint.second };
            }
        };

        for ( auto& Slot : m_BaseSlots )
            CheckForPick( Slot );
        for ( auto& Item : m_MenuItems )
            CheckForPick( Item );
    } else if ( PrimaryKey.isDown )
    {
        // Holding
        if ( m_MenuHoldingSprite != nullptr )
        {
            std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
            m_Camera->ScreenCoordToUICoord( HitPoint );

            glm::vec2 CurrentLocation = { HitPoint.first, HitPoint.second };
            glm::vec2 Delta           = CurrentLocation - m_MouseStartPosition;

            m_MenuHoldingSprite->Sprite->SetCoordinate( m_SpriteStartPosition + glm::vec3 { Delta, 0 } );
        }
    } else
    {
        // Possibly released
        if ( m_MenuHoldingSprite != nullptr )
        {
            std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
            m_Camera->ScreenCoordToUICoord( HitPoint );

            // Reset position by default
            m_MenuHoldingSprite->Sprite->SetCoordinate( m_SpriteStartPosition );

            const auto CheckForLand = [ &HitPoint, this ]( auto& Slot ) {
                if ( Slot.HitBox->HitTest( HitPoint ) )
                {
                    if ( Slot.Sprite == nullptr )
                    {
                        // Can't swap position automatically
                        const auto Center = Slot.HitBox->GetCenter( );
                        m_MenuHoldingSprite->Sprite->SetCoordinate( Center.first, Center.second );
                    }

                    m_MenuHoldingSprite->Swap( Slot );
                    m_MenuHoldingSprite = nullptr;

                    return true;
                }

                return false;
            };

            for ( auto& Slot : m_BaseSlots )
                if ( CheckForLand( Slot ) ) break;

            if ( m_MenuHoldingSprite != nullptr )
            {
                for ( auto& Slot : m_MenuItems )
                    if ( CheckForLand( Slot ) ) break;
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

void
GameManager::AddSlotHighlightUI( )
{
    if ( m_BaseSlotParticleParent )
    {
        m_BaseSlotParticleParent->Destroy( );
    }

    m_BaseSlots.clear( );
    m_BaseSlotParticleParent = AddConcept<Concept>( );

    auto SlotPAR = std::make_unique<ParticleAttributesRandomizer>( );
    SlotPAR->SetAngularVelocity( -10, 10 );

    const auto SlotBaseScale = glm::vec3( m_SlotScale * m_EditorAreaScale, m_SlotScale * m_EditorAreaScale, 1 );
    SlotPAR->SetLinearScale( SlotBaseScale - glm::vec3( 0.05F ), SlotBaseScale + glm::vec3( 0.05F ) );

    const auto SlotParticleCount         = 10;
    const auto ControlNodeSpriteLocation = m_BaseBoard->GetControlNodeSpriteLocation( );
    for ( const auto& [ ID, Location ] : ControlNodeSpriteLocation )
    {
        auto ScaledLocation = std::pair<FloatTy, FloatTy> { 0.5F, 0.5F } - Location;
        ScaledLocation      = { ScaledLocation.first * m_EditorAreaScale, ScaledLocation.second * m_EditorAreaScale };
        ScaledLocation      = std::pair<FloatTy, FloatTy> { 0.5F, 0.5F } - ScaledLocation;

        auto& PP = m_ParticlePools[ 1 ];

        const auto SlotCoordinate = m_EditingAreaCoord + m_BoardDimensions * glm::vec3( ScaledLocation.first, ScaledLocation.second, 1 );
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
        const auto ActualSize = SlotBaseScale * 512.F;
        auto&      Slot       = m_BaseSlots.emplace_back( );
        Slot.HitBox           = m_BaseSlotParticleParent->AddConcept<PureConceptAABBSquare>( SlotCoordinate.x - ActualSize.x / 2, SlotCoordinate.y - ActualSize.y / 2, ActualSize.x, ActualSize.y );
        Slot.ID               = "EmptySlot";
    }
}

void
SpriteHitBox::Swap( SpriteHitBox& Other, bool SwapCoordinate )
{
    if ( SwapCoordinate && Sprite != nullptr && Other.Sprite != nullptr )
    {
        const auto SpriteCoordinate      = Sprite->GetCoordinate( );
        const auto OtherSpriteCoordinate = Other.Sprite->GetCoordinate( );

        Sprite->SetCoordinate( OtherSpriteCoordinate );
        Other.Sprite->SetCoordinate( SpriteCoordinate );
    }

    std::swap( Sprite, Other.Sprite );
    std::swap( ID, Other.ID );

    // HitBox stays the same
}
