//
// Created by loyus on 7/31/2023.
//

#include "AudioHandle.hpp"

#ifdef USE_IRRKLANG
#include "AudioHandleImpl/AudioHandle_irrKlang.inl"
#elif defined( NO_AUDIO_ENGINE )
#    include "AudioHandleImpl/AudioHandleNone.inl"
#endif

