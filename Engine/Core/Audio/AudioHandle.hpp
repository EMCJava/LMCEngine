//
// Created by loyus on 7/31/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

#include <utility>

#include "Audio.hpp"

class AudioHandle
{
public:
    AudioHandle( )  = default;
    ~AudioHandle( ) = default;

    AudioHandle( NativeSoundHandle Handle );

    AudioHandle( AudioHandle&& Handle ) noexcept;
    AudioHandle& operator=( AudioHandle&& Handle ) noexcept;

    AudioHandle( AudioHandle& Handle )           = delete;
    AudioHandle operator=( AudioHandle& Handle ) = delete;

    /*
     *
     * Audio status getter
     *
     * */
    bool IsAudioEnded( );
    bool IsAudioPaused( );

    int64_t GetNativeCurrentAudioOffset( );

    /*
     *
     * Fetch GetNativeCurrentAudioOffset and add manual offset if necessary
     * Should call this every frame to keep result accurate
     *
     * */
    int64_t GetCorrectedCurrentAudioOffset( )
    {
        const auto TimeNow      = TimerTy::now( );
        const auto NativeOffset = GetNativeCurrentAudioOffset( );

        if ( m_NativeAudioOffset == NativeOffset )
        {
            const auto TimeSinceLastFetch = TimeNow - m_LastFetchTimePoint;

            const auto TimeDiffMillis     = std::chrono::duration_cast<std::chrono::milliseconds>( TimeSinceLastFetch ).count( );
            return m_NativeAudioOffset + TimeDiffMillis;
        }

        m_NativeAudioOffset  = NativeOffset;
        m_LastFetchTimePoint = TimeNow;

        return m_NativeAudioOffset;
    }

    /*
     *
     * Flow controls
     *
     * */
    void Paused( );
    void Resume( );

private:
    NativeSoundHandle m_NativeHandle;

    /*
     *
     * Offset returned by the audio engine might not be most up to date
     *
     * */
    int64_t             m_NativeAudioOffset { };
    TimerTy::time_point m_LastFetchTimePoint { };
};
