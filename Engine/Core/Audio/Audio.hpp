//
// Created by loyus on 7/30/2023.
//

#pragma once

#include <memory>

#ifdef USE_IRRKLANG
#	include <irrKlang.h>

using SoundHandle = std::shared_ptr<irrklang::ISound>;
using AudioSourceHandle = irrklang::ISoundSource *;
#endif
