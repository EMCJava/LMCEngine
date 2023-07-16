//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <glad/gl.h>

#define GL_CHECK(x)                                               \
	x;                                                            \
	{                                                             \
		auto GLError = gl->GetError();                            \
		if (GLError != GL_NO_ERROR) [[unlikely]]                  \
		{                                                         \
			spdlog::error("OpenGL error: {} on {}", GLError, #x); \
		}                                                         \
	}
