//
// Created by loyus on 6/23/2024.
//

#pragma once

#include "Engine/Core/Runtime/Assertion/Assertion.hpp"
#include "Engine/Core/Audio/AudioEngine.hpp"

#include <miniaudio.h>

#include <filesystem>
#include <stdexcept>

#define BackendInstance ( static_cast<ma_engine*>( m_BackendInstance ) )

AudioEngine::AudioEngine( )
{
    if ( g_AudioEngine != nullptr )
    {
        throw std::runtime_error( "There is already a AudioEngine instance!" );
    }

    m_BackendInstance = new ma_engine;

    ma_result Result;
    Result = ma_engine_init( nullptr, BackendInstance );
    if ( Result != MA_SUCCESS )
    {
        spdlog::critical( "Failed to initialize audio engine({}).", ma_result_description( Result ) );
        m_BackendInstance = nullptr;
    }

    g_AudioEngine = this;
}

AudioEngine::~AudioEngine( )
{
    REQUIRED( g_AudioEngine == this )

    if ( m_BackendInstance != nullptr )
    {
        ma_engine_uninit( BackendInstance );
        delete BackendInstance;
        m_BackendInstance = nullptr;
    }

    g_AudioEngine = nullptr;
}

NativeAudioSourceHandle
AudioEngine::CreateAudioHandle( std::string_view AudioPath )
{
    return std::string( AudioPath );
}

struct NativeSoundHandleDestructor {
    void operator( )( ma_sound* Ptr ) const
    {
        if ( Ptr != nullptr )
        {
            ma_sound_uninit( Ptr );
        }
    }
};

AudioHandle
AudioEngine::PlayAudio( NativeAudioSourceHandle AudioHandle, bool TrackAudio, bool PauseAtStart, bool Loop )
{
    ma_result Result;

    if ( !TrackAudio )
    {
        if ( Loop )
        {
            spdlog::error( "Failed to play looped audio untracked: {}", AudioHandle );
        }
        Result = ma_engine_play_sound( BackendInstance, AudioHandle.data( ), nullptr );
        if ( Result != MA_SUCCESS )
        {
            spdlog::error( "Failed to play audio: {}", AudioHandle );
        }

        return NativeSoundHandle { };
    }

    auto FullPath = std::filesystem::absolute( AudioHandle ).string( );
    auto Sound    = std::shared_ptr<ma_sound>( new ma_sound, NativeSoundHandleDestructor { } );
    Result        = ma_sound_init_from_file( BackendInstance, FullPath.c_str( ), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, nullptr, Sound.get( ) );
    if ( Result != MA_SUCCESS )
    {
        spdlog::error( "Failed to play audio: {}({})", AudioHandle, ma_result_description( Result ) );
        return NativeSoundHandle { };
    }

    ma_sound_set_looping( Sound.get( ), Loop );
    if ( !PauseAtStart ) ma_sound_start( Sound.get( ) );

    return Sound;
}

AudioHandle
AudioEngine::PlayAudio3D( NativeAudioSourceHandle AudioHandle, const glm::vec3& Position, bool TrackAudio, bool PauseAtStart, bool Loop )
{
    ma_result Result;

    if ( !TrackAudio )
    {
        spdlog::error( "Unable to play 3d audio untracked: {}", AudioHandle );

        if ( Loop )
        {
            spdlog::error( "Unable to play looped audio untracked: {}", AudioHandle );
        }
        Result = ma_engine_play_sound( BackendInstance, AudioHandle.data( ), nullptr );
        if ( Result != MA_SUCCESS )
        {
            spdlog::error( "Failed to play audio: {}({})", AudioHandle, ma_result_description( Result ) );
        }

        return NativeSoundHandle { };
    }

    auto Sound = std::shared_ptr<ma_sound>( new ma_sound, NativeSoundHandleDestructor { } );
    Result     = ma_sound_init_from_file( BackendInstance, AudioHandle.data( ), MA_SOUND_FLAG_DECODE, nullptr, nullptr, Sound.get( ) );
    if ( Result != MA_SUCCESS )
    {
        spdlog::error( "Failed to play audio: {}({})", AudioHandle, ma_result_description( Result ) );
        return NativeSoundHandle { };
    }

    ma_sound_set_looping( Sound.get( ), Loop );
    if ( !PauseAtStart ) ma_sound_start( Sound.get( ) );
    ma_sound_set_position( Sound.get( ), Position.x, Position.y, Position.z );

    return Sound;
}

void
AudioEngine::SetListenerPosition( const glm::vec3& Position, const glm::vec3& LookDirection, const glm::vec3& UpVector, const glm::vec3& VelPerSecond )
{
    ma_engine_listener_set_position( BackendInstance, 0, Position.x, Position.y, Position.z );
    ma_engine_listener_set_direction( BackendInstance, 0, LookDirection.x, LookDirection.y, LookDirection.z );
    ma_engine_listener_set_world_up( BackendInstance, 0, UpVector.x, UpVector.y, UpVector.z );
}
