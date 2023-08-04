//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptApplicable.hpp>
#include <Engine/Core/Audio/AudioEngine.hpp>

#include "TileSpriteSet.hpp"

class GameManager : public ConceptApplicable
{
    DECLARE_CONCEPT( GameManager, ConceptApplicable )

public:
    GameManager( );

    void
    Apply( ) override;

private:
    void
    LoadTileSprites( const std::set<uint32_t>& Degrees );

    void
    LoadTileMap( );

    void
    LoadAudio( );

    void
    SetupCamera( );

    void
    LoadPlayerSprites( );

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
    AudioHandle m_DelayCheckingHandle;
    AudioHandle m_MainAudioHandle;

    bool    m_IsCheckingDeviceDelay = true;
    int64_t m_UserDeviceOffsetMS { };

    TileSpriteSet* m_TileSpriteSet;

    /*
     *
     * Song settings
     *
     * */
    FloatTy m_BPM { };
    FloatTy m_BPMS { };

    /*
     *
     * Game State
     *
     * */
    bool                       m_ActivePlayerIsFire { false };
    class SpriteSquareTexture* m_ActivePlayerSprite { };
    class SpriteSquareTexture* m_InActivePlayerSprite { };
    /*
     *
     * Concept saves
     *
     * */
    class PureConceptCamera* m_Camera;

    class SpriteSquareTexture* FBSp { };
    class SpriteSquareTexture* IBSp { };
};