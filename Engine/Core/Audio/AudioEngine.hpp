//
// Created by loyus on 7/30/2023.
//

#pragma once

#include "Audio.hpp"
#include "AudioHandle.hpp"

#include <glm/vec3.hpp>

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

    /*
     *
     * Play audio sound with a handle, avoid real-time loading
     * Is TrackAudio == true, returns A sound handle of this sound instance
     *
     * */
    AudioHandle
    PlayAudio3D( NativeAudioSourceHandle AudioHandle, const glm::vec3& Position, bool TrackAudio = false, bool PauseAtStart = false, bool Loop = false );

    /*
     *
     * Useful for playing audio at a specific 3d location
     *
     * */
    void
    SetListenerPosition( const glm::vec3& Position, const glm::vec3& LookDirection, const glm::vec3& UpVector = { 0, 1, 0 }, const glm::vec3& VelPerSecond = { 0, 0, 0 } );

private:
    static inline AudioEngine* g_AudioEngine = nullptr;

    /*
     *
     * The base engine of this class
     *
     * */
    void* m_BackendInstance = nullptr;
};
