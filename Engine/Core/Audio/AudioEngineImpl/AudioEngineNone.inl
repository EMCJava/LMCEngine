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

NativeAudioSourceHandle
AudioEngine::CreateAudioHandle( std::string_view AudioPath )
{
    return nullptr;
}

AudioHandle
AudioEngine::PlayAudio( NativeAudioSourceHandle AudioHandle, bool TrackAudio, bool PauseAtStart, bool Loop )
{
    return nullptr;
}
