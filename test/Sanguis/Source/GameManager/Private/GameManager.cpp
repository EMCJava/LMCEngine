//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "BaseBoard.h"
#include "ControlNodeSimpleEffect.hpp"
#include "ControlNodePlaceholder.hpp"

#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Image/PureConceptImage.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptPerspectiveCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareTexture.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticlePool.hpp>
#include <Engine/Core/Graphic/Sprites/Particle/ParticleAttributesRandomizer.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>
#include <Engine/Core/Concept/PureConceptAABBSquare.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <fstream>
#include <sstream>
#include <ranges>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE_CHAINED( GameManager, PureConcept, m_Effect, TestInvokable, m_MenuItems, m_BaseSlots )

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

class Cube : public ConceptRenderable
{
    DECLARE_CONCEPT( Cube, ConceptRenderable )
public:
    Cube( )
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

        float vertices[] = {
            // front
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            // back
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

            // left
            -0.5f, -0.5f, 0.5f, 1.0f, 0.64453125f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 0.64453125f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.64453125f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.64453125f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 0.64453125f, 0.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 0.64453125f, 0.0f,

            // right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

            // up
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,

            // down
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f };

        GL_CHECK( gl->GenVertexArrays( 1, &m_VAO ) )
        GL_CHECK( gl->BindVertexArray( m_VAO ) )

        // 2. copy our vertices array in a buffer for OpenGL to use
        GL_CHECK( gl->GenBuffers( 1, &m_VBO ) )
        GL_CHECK( gl->BindBuffer( GL_ARRAY_BUFFER, m_VBO ) )
        GL_CHECK( gl->BufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW ) )

        // 3. then set our vertex attributes pointers
        GL_CHECK( gl->VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), nullptr ) )
        GL_CHECK( gl->EnableVertexAttribArray( 0 ) )

        GL_CHECK( gl->VertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( float ), (void*) ( 3 * sizeof( float ) ) ) )
        GL_CHECK( gl->EnableVertexAttribArray( 1 ) )
    }

    void
    Render( ) override
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )

        m_Shader->Bind( );
        REQUIRED_IF( m_ActiveCamera != nullptr && m_ActiveCamera->CanCastVT<PureConceptPerspectiveCamera>( ) )
        {
            auto* PC = (PureConceptPerspectiveCamera*) m_ActiveCamera;
            PC->UpdateProjectionMatrix( );
            // PC->SetCameraFacing( glm::normalize( PC->GetCameraFacing( ) + PC->CalculateCameraRightVector( ) * 0.005F * (float) Engine::GetEngine( )->GetUserInputHandle( )->GetCursorDeltaPosition( ).first ) );
            // PC->SetCameraUpVector( glm::normalize( PC->GetCameraUpVector( ) - PC->GetCameraFacing( ) * 0.005F * (float) Engine::GetEngine( )->GetUserInputHandle( )->GetCursorDeltaPosition( ).second ) );

            SetCameraMatrix( );
            m_Shader->SetMat4( "modelMatrix", m_Orientation.GetModelMatrix( ) );
            ApplyShaderUniforms( );

            m_Shader->Bind( );

            GL_CHECK( gl->Enable( GL_DEPTH_TEST ) )
            GL_CHECK( gl->DepthFunc( GL_LESS ) )
            GL_CHECK( gl->BindVertexArray( m_VAO ) )
            GL_CHECK( gl->DrawArrays( GL_TRIANGLES, 0, 3 * 2 * 6 ) )
            GL_CHECK( gl->Disable( GL_DEPTH_TEST ) )
        }
    }

    Orientation&
    GetOrientation( )
    {
        return m_Orientation;
    }

private:
    uint32_t    m_VAO { }, m_VBO { };
    Orientation m_Orientation;
};
DEFINE_CONCEPT_DS( Cube )

class RotatingCube : public ConceptApplicable
{
    DECLARE_CONCEPT( RotatingCube, ConceptApplicable )
public:
    RotatingCube( )
    {
        m_Cube = AddConcept<Cube>( );
        m_Cube->SetShader( Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorPreVertexShader" ) );
        // m_Cube->GetOrientation( ).AlterRotation( 0, 3.141592 / 2, 0 );

        SetSearchThrough( );
    }

    void Apply( ) override
    {
        const auto DeltaTime = Engine::GetEngine( )->GetDeltaSecond( );
        m_Cube->GetOrientation( ).AlterRotation( -DeltaTime, DeltaTime * 1.2, DeltaTime * 1.4 );
    }

private:
    std::shared_ptr<Cube> m_Cube;
};
DEFINE_CONCEPT_DS( RotatingCube )

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );
    auto DefaultShader           = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" );
    auto DefaultShaderInstancing = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShaderInstancing" );

    constexpr std::pair<int, int> WindowSize = { 1920, 1080 };
    Engine::GetEngine( )->SetLogicalMainWindowViewPortDimensions( WindowSize );

    {
        m_UICamera = AddConcept<PureConceptOrthoCamera>( );
        m_UICamera->SetRuntimeName( "UI Camera" );
        m_UICamera->PushToCameraStack( );

        m_UICamera->SetCoordinate( WindowSize.first / 2, WindowSize.second / 2 );
    }

    {
        m_MainCamera = AddConcept<PureConceptPerspectiveCamera>( );
        m_MainCamera->SetRuntimeName( "Main Camera" );
        m_MainCamera->PushToCameraStack( );

        m_MainCamera->SetCameraPerspectiveFOV( 30, false );
        m_MainCamera->SetCameraPosition( glm::vec3( 10.0f, 40.0f, 40.0f ), false );
        m_MainCamera->SetCameraFacing( glm::vec3( 0.0f, -1.41421356237f, -1.41421356237f ), false );
        m_MainCamera->SetCameraUpVector( glm::vec3( 0.0f, 1.0f, 0.0f ), false );
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

            auto SystemUICamera = m_UICanvas->AddConcept<PureConceptOrthoCamera>( );
            SystemUICamera->SetRuntimeName( "System UI Camera" );
            SystemUICamera->PushToCameraStack( );

            m_UICanvas->SetCanvasCamera( SystemUICamera );
        }
    }

    m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/Inv.png" ) );

    {
        auto Sp           = m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/Boards/StarterBoard.png" ) );
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
        auto Sp = m_WandEditorCanvas->AddConcept<SpriteSquareTexture>( DefaultShader, std::make_shared<PureConceptImage>( "Assets/Texture/UI/wand.png" ) );
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

    {
        const auto& PerspectiveCanvas = AddConcept<Canvas>( );
        PerspectiveCanvas->SetRuntimeName( "Perspective Canvas" );
        PerspectiveCanvas->SetCanvasCamera( m_MainCamera );

        PerspectiveCanvas->AddConcept<RotatingCube>( );

        PerspectiveCanvas->GetConceptAt<RotatingCube>( 0 )->GetConceptAt<Cube>( 0 )->GetOrientation( ).SetCoordinate( 5, 3, 5 );

        auto DefaultColorPreVertexShader = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultColorPreVertexShader" );
        for ( int h = 0; h < 5; ++h )
        {
            for ( int i = 0; i < 11; ++i )
            {
                for ( int j = 0; j < 11; ++j )
                {
                    if ( i == 0 || i == 10 || j == 0 || j == 10 )
                    {
                        auto CubeInstance = PerspectiveCanvas->AddConcept<Cube>( );
                        CubeInstance->SetShader( DefaultColorPreVertexShader );
                        CubeInstance->GetOrientation( ).SetCoordinate( i, h, j );
                    }
                }
            }
        }
    }

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

        auto* Pa = &PP->AddParticle( );
        m_PAR->Apply( *Pa );
        Pa->GetOrientation( ).SetOrigin( m_ParticleScriptsize / 2, m_ParticleScriptsize / 2 );
        Pa->GetOrientation( ).SetCoordinate( HitPoint.first, HitPoint.second );
    }
}

void
GameManager::TestInvokable( )
{
    auto& PP = m_ParticlePools[ rand( ) % m_ParticlePools.size( ) ];

    auto* Pa = &PP->AddParticle( );
    m_PAR->Apply( *Pa );
    Pa->GetOrientation( ).SetOrigin( m_ParticleScriptsize / 2, m_ParticleScriptsize / 2 );
    Pa->GetOrientation( ).SetCoordinate( 0, 0 );
}

void
GameManager::AddSlotHighlightUI( )
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
            auto* Pa = &PP->AddParticle( );
            SlotPAR->Apply( *Pa );

            Pa->SetLifeTime( std::numeric_limits<FloatTy>::infinity( ) );
            Pa->GetColor( ) = glm::vec4( 1 );
            Pa->GetOrientation( ).SetOrigin( m_ParticleScriptsize / 2, m_ParticleScriptsize / 2 );
            Pa->GetOrientation( ).SetCoordinate( SlotCoordinate );
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
