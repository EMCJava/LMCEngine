//
// Created by loyus on 6/23/2024.
//

#include "Engine/Core/Audio/AudioHandle.hpp"

#include <miniaudio.h>

#include <spdlog/spdlog.h>

AudioHandle::AudioHandle( NativeSoundHandle Handle )
    : m_NativeHandle( std::move( Handle ) )
{ }

AudioHandle::AudioHandle( AudioHandle&& Handle ) noexcept
    : m_NativeHandle( std::move( Handle.m_NativeHandle ) )
{ }

AudioHandle&
AudioHandle::operator=( AudioHandle&& Handle ) noexcept
{
    m_NativeHandle = std::move( Handle.m_NativeHandle );
    return *this;
}

int64_t
AudioHandle::GetNativeCurrentAudioOffset( )
{
    int64_t Time = ma_sound_get_time_in_milliseconds( m_NativeHandle.get( ) );
    if ( Time < 0 ) spdlog::error( "ma_sound_get_time_in_milliseconds < 0, could be int overflow" );
    return Time;
}

bool
AudioHandle::IsAudioEnded( )
{
    return ma_sound_at_end( m_NativeHandle.get( ) );
}

bool
AudioHandle::IsAudioPaused( )
{
    return !ma_sound_is_playing( m_NativeHandle.get( ) );
}

void
AudioHandle::Paused( )
{
    ma_sound_stop( m_NativeHandle.get( ) );
}

void
AudioHandle::Resume( )
{
    ma_sound_start( m_NativeHandle.get( ) );
}

bool
AudioHandle::SetSpeed( FloatTy Speed )
{
    ma_sound_set_pitch( m_NativeHandle.get( ), Speed );
    return true;
}

void
AudioHandle::SetVolume( float Volume )
{
    ma_sound_set_volume( m_NativeHandle.get( ), Volume );
}

bool
AudioHandle::SetPlayOffset( uint32_t Milliseconds )
{
    ma_sound_seek_to_pcm_frame( m_NativeHandle.get( ), ( Milliseconds * ma_engine_get_sample_rate( ma_sound_get_engine( m_NativeHandle.get( ) ) ) ) / 1000 );
    return true;
}
