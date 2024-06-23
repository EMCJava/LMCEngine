//
// Created by loyus on 7/30/2023.
//

#include "AudioEngine.hpp"

#ifdef USE_IRRKLANG
#    include "AudioEngineImpl/AudioEngine_irrKlang.inl"
#elif defined( USE_MINIAUDIO )
#    define MINIAUDIO_IMPLEMENTATION
#    include "AudioEngineImpl/AudioEngine_miniaudio.inl"
#elif defined( NO_AUDIO_ENGINE )
#    include "AudioEngineImpl/AudioEngineNone.inl"
#endif
