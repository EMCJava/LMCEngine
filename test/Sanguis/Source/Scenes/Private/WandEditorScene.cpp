//
// Created by samsa on 10/24/2023.
//

#include "WandEditorScene.hpp"
#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"
#include "ControlNodePlaceholder.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticleAttributesRandomizer.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticlePool.hpp>
#include <Engine/Core/Concept/PureConceptAABBSquare.hpp>
#include <Engine/Core/UI/RectButton.hpp>

#include <fstream>
#include <sstream>

DEFINE_CONCEPT_DS( WandEditorScene )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( WandEditorScene, ConceptApplicable, m_Effect, m_MenuItems, m_BaseSlots )

DEFINE_SIMPLE_IMGUI_TYPE( NodeSpritePair, NodeName )
DEFINE_SIMPLE_IMGUI_TYPE( SpriteHitBox, Data, SlotName )

namespace
{
inline std::pair<FloatTy, FloatTy>
operator-( const std::pair<FloatTy, FloatTy>& lhs, const std::pair<FloatTy, FloatTy>& rhs )
{
    return std::make_pair( lhs.first - rhs.first, lhs.second - rhs.second );
}

std::string
GetTexturePath( Element ElementType )
{
    return "Assets/Texture/Elements/" + ToString( ElementType ) + ".png";
}
}   // namespace

WandEditorScene::WandEditorScene( )
{
    SetSearchThrough( );

    auto DefaultShader           = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" );
    auto DefaultShaderInstancing = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureInstancingShader" );

    const auto WindowSize = Engine::GetEngine( )->GetLogicalMainWindowViewPortDimensions( );

    {
        m_UICamera = AddConcept<PureConceptOrthoCamera>( );
        m_UICamera->SetRuntimeName( "UI Camera" );
        m_UICamera->PushToCameraStack( );

        m_UICamera->SetCoordinate( WindowSize.first / 2, WindowSize.second / 2 );
    }


    {
        m_WandEditorCanvas = AddConcept<Canvas>( );
        m_WandEditorCanvas->SetRuntimeName( "Wand Editor UI Canvas" );
        m_WandEditorCanvas->SetCanvasCamera( m_UICamera );

        m_BaseSlotMenuCanvas = AddConcept<Canvas>( );
        m_BaseSlotMenuCanvas->SetRuntimeName( "BaseSlot Menu" );
        m_BaseSlotMenuCanvas->SetCanvasCamera( m_UICamera );

        m_ParticleCanvas = AddConcept<Canvas>( );
        m_ParticleCanvas->SetRuntimeName( "Particle Canvas" );
        m_ParticleCanvas->SetCanvasCamera( m_UICamera );

        {
            m_UICanvas = AddConcept<Canvas>( );
            m_UICanvas->SetRuntimeName( "UI Canvas" );

            auto SystemUICamera = m_UICanvas->AddConcept<PureConceptOrthoCamera>( ).Get( );
            SystemUICamera->SetRuntimeName( "System UI Camera" );
            SystemUICamera->PushToCameraStack( );

            m_UICanvas->SetCanvasCamera( SystemUICamera );
        }
    }

    m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/Inv.png" ) );

    {
        auto Sp           = m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/Boards/StarterBoard.png" ) ).Get( );
        m_BoardDimensions = { Sp->GetSpriteDimensions( ).first, Sp->GetSpriteDimensions( ).second, 0 };
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( m_EditorAreaScale, m_EditorAreaScale );
        Sp->SetCoordinate( m_EditingAreaCoord + m_BoardDimensions / 2.F );
    }

    {
        /*
         *
         * Particle setup
         *
         * */
        m_PAR = std::make_unique<ParticleAttributesRandomizer>( );
        m_PAR->SetLinearScale( { 0.01, 0.01, 0.01 }, { 0.1, 0.1, 0.1 } );
        m_PAR->SetVelocity( { -20, -20, 0 }, { 20, 20, 0 } );
        m_PAR->SetAngularVelocity( -31.415F, 31.415F );
        m_PAR->SetStartLinearColor( glm::vec4( 1, 0, 0, 0.9 ), glm::vec4( 1, 0.2, 0, 1 ) );
        m_PAR->SetEndLinearColor( glm::vec4( 1, 0.6, 0.4, 0 ), glm::vec4( 1, 0.8, 0.6, 0 ) );
        m_PAR->SetLifetime( 1, 5 );

        auto Image = std::make_shared<PureConceptImage>( "Assets/Texture/Particle/star.png" );
        REQUIRED( Image->GetImageDimension( ) == ( std::pair<int32_t, int32_t>( m_ParticleScriptsize, m_ParticleScriptsize ) ) )
        m_ParticlePools.push_back( m_ParticleCanvas->AddConcept<ParticlePool>( ) );
        m_ParticlePools.back( )->SetSprite( std::make_shared<SpriteSquareTexture>( DefaultShaderInstancing, std::move( Image ) ) );

        Image = std::make_shared<PureConceptImage>( "Assets/Texture/Particle/ring.png" );
        REQUIRED( Image->GetImageDimension( ) == ( std::pair<int32_t, int32_t>( m_ParticleScriptsize, m_ParticleScriptsize ) ) )
        m_ParticlePools.push_back( m_ParticleCanvas->AddConcept<ParticlePool>( ) );
        m_ParticlePools.back( )->SetSprite( std::make_shared<SpriteSquareTexture>( DefaultShaderInstancing, std::move( Image ) ) );
    }

    /*
     *
     * Wand edit system setup
     *
     * */
    {
        std::ifstream     BoardTemp( "Assets/Boards/StarterBoard.yaml" );
        std::stringstream buffer;
        buffer << BoardTemp.rdbuf( );

        m_BaseBoard = std::make_unique<SaBaseBoard>( );
        m_BaseBoard->Serialize( buffer.str( ) );

        AddSlotHighlightUI( );
    }

    {
        auto Sp = m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/wand.png" ) ).Get( );
        Sp->SetCenterAsOrigin( );

        Sp->SetScale( m_EditorAreaScale, m_EditorAreaScale );
        Sp->SetCoordinate( m_EditingAreaCoord + m_BoardDimensions / 2.F );
    }

    constexpr FloatTy UIIconSize  = 512;
    glm::vec3         TopLeftMenu = { 230, 860, 0 };
    glm::vec3         IconSize { UIIconSize * m_EditorAreaScale * m_IconScale };

    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 6; ++j )
        {
            auto& Record = m_MenuItems.emplace_back( );

            const auto ElementType = Element( rand( ) % NumOfElement );
            const auto TexturePath = GetTexturePath( ElementType );

            auto Image = std::make_shared<PureConceptImage>( TexturePath );
            REQUIRED( Image->GetImageDimension( ) == ( std::pair<int32_t, int32_t>( UIIconSize, UIIconSize ) ) )

            Record.Data.Node   = std::make_shared<SaControlNodeSimpleEffect>( SaEffect { true, ElementType, 1 } );
            Record.Data.Sprite = m_BaseSlotMenuCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::move( Image ) );
            m_BoardDimensions  = { Record.Data.Sprite->GetSpriteDimensions( ).first, Record.Data.Sprite->GetSpriteDimensions( ).second, 0 };
            Record.Data.Sprite->SetCenterAsOrigin( );

            const auto FinalScale = m_EditorAreaScale * m_IconScale;
            Record.Data.Sprite->SetScale( FinalScale, FinalScale );

            const auto MenuCoordinate = TopLeftMenu + IconSize * glm::vec3 { i, -j, 0 };
            Record.Data.Sprite->SetCoordinate( MenuCoordinate );

            const auto ActualSize = FinalScale * UIIconSize;
            Record.HitBox         = m_BaseSlotMenuCanvas->AddConcept<PureConceptAABBSquare>( MenuCoordinate.x - ActualSize / 2, MenuCoordinate.y - ActualSize / 2, ActualSize, ActualSize );

            Record.Data.NodeName = ToString( ElementType );
        }
    }

    m_Effect = std::make_unique<SaEffect>( );

    {
        m_UpdateSlotsButton = m_UICanvas->AddConcept<RectButton>( -25, -12 );
        m_UpdateSlotsButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
        m_UpdateSlotsButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
        m_UpdateSlotsButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
        m_UpdateSlotsButton->SetText( "Update" );
        m_UpdateSlotsButton->SetPivot( 0.5F, 0.5F );
        m_UpdateSlotsButton->SetCoordinate( -WindowSize.first * 5.5 / 18, -WindowSize.second * 6 / 18 );
        m_UpdateSlotsButton->SetActiveCamera( m_UICamera.get( ) );
        m_UpdateSlotsButton->SetCallback( [ this ]( ) {
            for ( const SpriteHitBox& Slot : m_BaseSlots )
            {
                m_BaseBoard->SetSlot( Slot.SlotName, Slot.Data.Node );
            }

            m_BaseBoard->GetEffect( *m_Effect );
            spdlog::info( "Effect.Iteration : {}", m_Effect->Iteration );

            std::stringstream ResultStr;
            for ( const auto& Count : m_Effect->ElementCount )
            {
                ResultStr << (int) Count << ' ';
            }

            spdlog::info( "Effect.ElementCounts : {}", ResultStr.str( ) );
        } );
    }
}

void
WandEditorScene::Apply( )
{
    const auto& PrimaryKey = Engine::GetEngine( )->GetUserInputHandle( )->GetPrimaryKey( );
    if ( PrimaryKey.isPressed )
    {
        // First press
        std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
        m_UICamera->ScreenCoordToWorldCoord( HitPoint );

        const auto CheckForPick = [ &HitPoint, this ]( auto& Slot ) {
            if ( Slot.Data.Sprite != nullptr && Slot.HitBox->HitTest( HitPoint ) )
            {
                spdlog::info( "BaseSlot HitTest : {}", Slot.Data.NodeName );
                m_MenuHoldingSprite   = &Slot;
                m_SpriteStartPosition = Slot.Data.Sprite->GetCoordinate( );
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
            m_UICamera->ScreenCoordToWorldCoord( HitPoint );

            glm::vec2 CurrentLocation = { HitPoint.first, HitPoint.second };
            glm::vec2 Delta           = CurrentLocation - m_MouseStartPosition;

            m_MenuHoldingSprite->Data.Sprite->SetCoordinate( m_SpriteStartPosition + glm::vec3 { Delta, 0 } );
        }
    } else
    {
        // Possibly released
        if ( m_MenuHoldingSprite != nullptr )
        {
            std::pair<FloatTy, FloatTy> HitPoint = Engine::GetEngine( )->GetUserInputHandle( )->GetCursorPosition( );
            m_UICamera->ScreenCoordToWorldCoord( HitPoint );

            // Reset position by default
            m_MenuHoldingSprite->Data.Sprite->SetCoordinate( m_SpriteStartPosition );

            const auto CheckForLand = [ &HitPoint, this ]( auto& Slot ) {
                if ( Slot.HitBox->HitTest( HitPoint ) )
                {
                    if ( Slot.Data.Sprite == nullptr )
                    {
                        // Can't swap position automatically
                        const auto Center = Slot.HitBox->GetCenter( );
                        m_MenuHoldingSprite->Data.Sprite->SetCoordinate( Center.first, Center.second );
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
        m_UICamera->ScreenCoordToWorldCoord( HitPoint );

        auto& PP = m_ParticlePools[ rand( ) % 2 ];

        PP->AddParticle( [ this, &HitPoint ]( Particle& Pa ) {
            m_PAR->Apply( Pa );
            Pa.GetOrientation( ).SetOrigin( m_ParticleScriptsize / 2, m_ParticleScriptsize / 2, 0 );
            Pa.GetOrientation( ).Coordinate = { HitPoint.first, HitPoint.second, 0 };
        } );
    }
}

void
WandEditorScene::AddSlotHighlightUI( )
{
    m_BaseSlots.clear( );

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
            PP->AddParticle( [ this, &SlotPAR, &SlotCoordinate ]( auto& Pa ) {
                SlotPAR->Apply( Pa );
                Pa.SetLifeTime( std::numeric_limits<FloatTy>::infinity( ) );
                Pa.GetColor( ) = glm::vec4( 1 );
                Pa.GetOrientation( ).SetOrigin( m_ParticleScriptsize / 2, m_ParticleScriptsize / 2, 0 );
                Pa.GetOrientation( ).Coordinate = SlotCoordinate;
            } );
        }

        // Hit box
        const auto ActualSize = SlotBaseScale * m_ParticleScriptsize;
        auto&      Slot       = m_BaseSlots.emplace_back( );
        Slot.HitBox           = m_WandEditorCanvas->AddConcept<PureConceptAABBSquare>( SlotCoordinate.x - ActualSize.x / 2, SlotCoordinate.y - ActualSize.y / 2, ActualSize.x, ActualSize.y );
        Slot.SlotName         = ID;

        Slot.Data.Node = std::make_unique<SaControlNodePlaceholder>( );
    }
}


void
SpriteHitBox::Swap( SpriteHitBox& Other, bool SwapCoordinate )
{
    Data.Swap( Other.Data, SwapCoordinate );

    // HitBox stays the same
}

void
NodeSpritePair::Swap( NodeSpritePair& Other, bool SwapCoordinate )
{
    if ( SwapCoordinate && Sprite != nullptr && Other.Sprite != nullptr )
    {
        const auto SpriteCoordinate      = Sprite->GetCoordinate( );
        const auto OtherSpriteCoordinate = Other.Sprite->GetCoordinate( );

        Sprite->SetCoordinate( OtherSpriteCoordinate );
        Other.Sprite->SetCoordinate( SpriteCoordinate );
    }

    std::swap( Sprite, Other.Sprite );
    std::swap( Node, Other.Node );
    std::swap( NodeName, Other.NodeName );
}
