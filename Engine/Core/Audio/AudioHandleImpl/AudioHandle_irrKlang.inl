//
// Created by loyus on 8/1/2023.
//

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
AudioHandle::GetCurrentAudioOffset( )
{
    return m_NativeHandle->getPlayPosition( );
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