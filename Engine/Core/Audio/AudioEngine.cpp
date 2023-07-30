//
// Created by loyus on 7/30/2023.
//

#include "AudioEngine.hpp"

#include <irrKlang.h>
using namespace irrklang;

#include <stdexcept>

#define BackendInstance ((ISoundEngine *)m_BackendInstance)

AudioEngine::AudioEngine()
{
	if (g_AudioEngine != nullptr)
	{
		throw std::runtime_error("There is already a AudioEngine instance!");
	}

	m_BackendInstance = createIrrKlangDevice();

	g_AudioEngine = this;
}

AudioEngine::~AudioEngine()
{
	if (g_AudioEngine != this)
	{
		throw std::runtime_error("AudioEngine could be destructed more then once!");
	}

	BackendInstance->drop();
	m_BackendInstance = nullptr;

	g_AudioEngine = nullptr;
}

AudioSourceHandle
AudioEngine::CreateAudioHandle(std::string_view AudioPath)
{
	return BackendInstance->addSoundSourceFromFile(AudioPath.data());
}

struct SoundHandleDestructor {
	void operator()(irrklang::ISound* Ptr) const {
		if(Ptr != nullptr)
		{
			Ptr->drop();
		}
	}
};

SoundHandle
AudioEngine::PlayAudio(AudioSourceHandle AudioHandle, bool TrackAudio)
{
	return SoundHandle{BackendInstance->play2D(AudioHandle, false, false, TrackAudio), SoundHandleDestructor{}};
}
