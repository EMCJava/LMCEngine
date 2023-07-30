//
// Created by loyus on 7/30/2023.
//

#pragma once

#include "Audio.hpp"

#include <string_view>

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();

	AudioSourceHandle
	CreateAudioHandle(std::string_view AudioPath);

	/*
	 *
	 * Play audio sound with a handle, avoid real-time loading
	 * Is TrackAudio == true, returns A sound handle of this sound instance
	 *
	 * */
	SoundHandle
	PlayAudio(AudioSourceHandle AudioHandle, bool TrackAudio = false);

private:
	static inline AudioEngine *g_AudioEngine = nullptr;

	/*
	 *
	 * The base engine of this class
	 *
	 * */
	void *m_BackendInstance = nullptr;
};
