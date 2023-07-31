//
// Created by loyus on 7/30/2023.
//

#pragma once

#include "Audio.hpp"
#include "AudioHandle.hpp"

#include <string_view>

class AudioEngine
{
public:
    AudioEngine( );
    ~AudioEngine( );

    NativeAudioSourceHandle
    CreateAudioHandle( std::string_view AudioPath );

    /*
     *
     * Play audio sound with a handle, avoid real-time loading
     * Is TrackAudio == true, returns A sound handle of this sound instance
     *
     * */
    AudioHandle
    PlayAudio( NativeAudioSourceHandle AudioHandle, bool TrackAudio = false, bool PauseAtStart = false, bool Loop = false );

private:
    static inline AudioEngine* g_AudioEngine = nullptr;

    /*
     *
     * The base engine of this class
     *
     * */
    void* m_BackendInstance = nullptr;
};
