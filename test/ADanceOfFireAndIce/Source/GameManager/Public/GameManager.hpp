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
    LoadTileSprites( std::set<uint32_t> Degrees );

    void
    LoadTileMap( );

    void
    LoadAudio( );

    void
    SetupCamera( );

private:
    AudioHandle m_DelayCheckingHandle;
    AudioHandle m_MainAudioHandle;

    bool    m_IsCheckingDeviceDelay = true;
    int64_t m_UserDeviceOffsetMS { };

    TileSpriteSet* m_TileSpriteSet;

    /*
     *
     * Concept saves
     *
     * */
    class PureConceptCamera* m_Camera;
};