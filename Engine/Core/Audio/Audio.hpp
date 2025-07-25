//
// Created by loyus on 7/30/2023.
//

#pragma once

#include <memory>

#ifdef USE_IRRKLANG

#    include <irrKlang.h>

using NativeSoundHandle       = std::shared_ptr<irrklang::ISound>;
using NativeAudioSourceHandle = irrklang::ISoundSource*;

#elif defined( NO_AUDIO_ENGINE )

using NativeSoundHandle       = void*;
using NativeAudioSourceHandle = void*;

#else

#    error "No audio engine defined"

#endif
