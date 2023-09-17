//
// Created by loyus on 7/30/2023.
//

#pragma once

#include "Engine/Core/Runtime/Assertion/Assertion.hpp"

#include <irrKlang.h>
using namespace irrklang;

#include <stdexcept>

#define BackendInstance ( static_cast<ISoundEngine*>( m_BackendInstance ) )

AudioEngine::AudioEngine( )
{
    if ( g_AudioEngine != nullptr )
    {
        throw std::runtime_error( "There is already a AudioEngine instance!" );
    }

    m_BackendInstance = createIrrKlangDevice( );

    g_AudioEngine = this;
}

AudioEngine::~AudioEngine( )
{
    REQUIRED( g_AudioEngine == this )

    BackendInstance->drop( );
    m_BackendInstance = nullptr;

    g_AudioEngine = nullptr;
}

NativeAudioSourceHandle
AudioEngine::CreateAudioHandle( std::string_view AudioPath )
{
    return BackendInstance->addSoundSourceFromFile( AudioPath.data( ) );
}

struct NativeSoundHandleDestructor {
    void operator( )( irrklang::ISound* Ptr ) const
    {
        if ( Ptr != nullptr )
        {
            Ptr->drop( );
        }
    }
};

AudioHandle
AudioEngine::PlayAudio( NativeAudioSourceHandle AudioHandle, bool TrackAudio, bool PauseAtStart, bool Loop )
{
    return NativeSoundHandle { BackendInstance->play2D( AudioHandle, Loop, PauseAtStart, TrackAudio ), NativeSoundHandleDestructor {} };
}
