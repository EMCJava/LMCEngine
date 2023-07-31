//
// Created by loyus on 8/1/2023.
//

AudioHandle::AudioHandle( NativeSoundHandle Handle ) { }

AudioHandle::AudioHandle( AudioHandle&& ) noexcept
{ }

AudioHandle&
AudioHandle::operator=( AudioHandle&& ) noexcept
{
    return *this;
}

int64_t
AudioHandle::GetCurrentAudioOffset( )
{
    return 0;
}

bool
AudioHandle::IsAudioEnded( )
{
    return true;
}

bool
AudioHandle::IsAudioPaused( )
{
    return true;
}

void
AudioHandle::Paused( )
{
}

void
AudioHandle::Resume( )
{
}