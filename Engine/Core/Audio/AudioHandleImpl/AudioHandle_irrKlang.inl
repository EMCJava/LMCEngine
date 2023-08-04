//
// Created by loyus on 8/1/2023.
//

#include "Engine/Core/Audio/AudioHandle.hpp"

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
    const auto Position = m_NativeHandle->getPlayPosition( );
    return Position == -1 ? 0 : Position;
}

bool
AudioHandle::IsAudioEnded( )
{
    return m_NativeHandle->isFinished( );
}

bool
AudioHandle::IsAudioPaused( )
{
    return m_NativeHandle->getIsPaused( );
}

void
AudioHandle::Paused( )
{
    return m_NativeHandle->setIsPaused( true );
}

void
AudioHandle::Resume( )
{
    return m_NativeHandle->setIsPaused( false );
}

bool
AudioHandle::SetSpeed( FloatTy Speed )
{
    return m_NativeHandle->setPlaybackSpeed( Speed );
}
