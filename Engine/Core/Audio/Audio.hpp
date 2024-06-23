//
// Created by loyus on 7/30/2023.
//

#pragma once

#include <memory>

#ifdef USE_IRRKLANG

#    include <irrKlang.h>

using NativeSoundHandle       = std::shared_ptr<irrklang::ISound>;
using NativeAudioSourceHandle = irrklang::ISoundSource*;

#elif defined( USE_MINIAUDIO )
#include <string>
using NativeSoundHandle       = std::shared_ptr<class ma_sound>;
using NativeAudioSourceHandle = std::string;

#elif defined( NO_AUDIO_ENGINE )

using NativeSoundHandle       = void*;
using NativeAudioSourceHandle = void*;

#else

#    error "No audio engine defined"

#endif
