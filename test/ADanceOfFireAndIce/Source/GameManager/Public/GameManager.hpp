//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>
#include <Engine/Core/Audio/AudioEngine.hpp>

#include "TileSpriteSet.hpp"

class GameManager : public ConceptList
{
    DECLARE_CONCEPT( GameManager, ConceptList )

    /*
     *
     * Tolerance in degrees
     *
     * */
    enum class Tolerance : uint16_t {
        Perfect           = 30,
        Good              = 45,
        Bad               = 55,
        ToleranceBarRange = 65,
        EarlyMiss         = 70,
        Miss,
        None
    };

    /*
     *
     *
     * Has to calculated base on bpm
     * */
    Tolerance
    ToTolerance( FloatTy DeltaTime );

public:
    GameManager( );

    void
    Apply( ) override;

private:
    /*
     *
     * Called by Apply
     *
     * */
    void
    ApplyOffsetWizard( );

    /*
     *
     * Loaders
     *
     * */
    void
    LoadTileSprites( const std::set<uint32_t>& Degrees );

    void
    LoadTileMap( );

    void
    LoadAudio( );

    void
    SetupShader( );

    void
    SetupCamera( );

    void
    SetupExplosionSpriteTemplate( );

    void
    LoadPlayerSprites( );

    void
    LoadToleranceSprite( );

    void
    SetBPM( FloatTy BPM );

    /*
     *
     * Game procedure
     *
     * */
    void
    UpdateDeviceOffset( );

    /*
     *
     * User Input
     *
     * */
    bool
    TryAlterPlayer( );

    bool IsUserPrimaryInteract( );

private:
    AudioHandle             m_DelayCheckingHandle;
    NativeAudioSourceHandle m_DelayCheckingSoundSource;
    AudioHandle             m_MainAudioHandle;
    NativeAudioSourceHandle m_NoteHitSfxSource;

    std::shared_ptr<class Canvas> m_UICanvas;

    bool    m_IsCheckingDeviceDelay = false;
    bool    m_Playing               = false;
    int64_t m_UserDeviceOffsetMS { };

    std::weak_ptr<TileSpriteSet> m_TileSpriteSetRef;

    /*
     *
     * Song settings
     *
     * */
    FloatTy m_BPM { };
    FloatTy m_MSPB { };
    FloatTy m_DegreePreMS { };
    FloatTy m_PlayingSpeed { 1 };

    /*
     *
     * Game State
     *
     * */
    bool                                       m_ActivePlayerIsFire { false };
    std::shared_ptr<class SpriteSquareTexture> m_ActivePlayerSprite { };
    std::shared_ptr<class SpriteSquareTexture> m_InActivePlayerSprite { };
    std::shared_ptr<class RectButton>          m_StartButton { };
    std::shared_ptr<class RectButton>          m_OffsetWizardButton { };

    bool m_PlayerDirectionClockWise { true };

    bool m_WaitingForFirstBeat { true };

    bool m_IsAutoPlay { true };

    /*
     *
     * Linear interpolation for camera movement
     *
     * */
    glm::vec2 m_CameraStart { }, m_CameraEnd { };
    FloatTy   m_CameraLerp { 1 };

    /*
     *
     * Concept saves
     *
     * */
    std::shared_ptr<class PureConceptOrthoCamera> m_Camera { };

    std::weak_ptr<class SpriteSquareTexture> m_FBSp { };
    std::weak_ptr<class SpriteSquareTexture> m_IBSp { };

    std::shared_ptr<class Shader>                      m_SpriteShader { };
    std::unique_ptr<class SpriteSquareAnimatedTexture> m_ExplosionSpriteTemplate;

    std::shared_ptr<class ToleranceBar> m_ToleranceBar { };

    static constexpr auto m_ManualMapping = false;

    ENABLE_IMGUI( GameManager )
};