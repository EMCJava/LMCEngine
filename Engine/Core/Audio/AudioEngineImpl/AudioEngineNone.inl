//
// Created by loyus on 7/30/2023.
//

#pragma once

#include <stdexcept>

AudioEngine::AudioEngine( )
{
    if ( g_AudioEngine != nullptr )
    {
        throw std::runtime_error( "There is already a AudioEngine instance!" );
    }

    g_AudioEngine = this;
}

AudioEngine::~AudioEngine( )
{

    m_BackendInstance = nullptr;
    g_AudioEngine     = nullptr;
}

AudioSourceHandle
AudioEngine::CreateAudioHandle( std::string_view AudioPath )
{
    return nullptr;
}

SoundHandle
AudioEngine::PlayAudio( AudioSourceHandle AudioHandle, bool TrackAudio )
{
    return nullptr;
}
