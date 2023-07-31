//
// Created by loyus on 7/31/2023.
//

#pragma once

#include <utility>

#include "Audio.hpp"

class AudioHandle
{
public:
    AudioHandle( )  = default;
    ~AudioHandle( ) = default;

    AudioHandle( NativeSoundHandle Handle );

    AudioHandle( AudioHandle&& Handle ) noexcept ;
    AudioHandle& operator=( AudioHandle&& Handle ) noexcept ;

    AudioHandle( AudioHandle& Handle ) = delete;
    AudioHandle operator=( AudioHandle& Handle ) = delete;

    /*
     *
     * Audio status getter
     *
     * */
    bool IsAudioEnded( );
    bool IsAudioPaused( );

    int64_t GetCurrentAudioOffset( );

    /*
     *
     * Flow controls
     *
     * */
    void Paused( );
    void Resume( );

private:
    NativeSoundHandle m_NativeHandle;
};
