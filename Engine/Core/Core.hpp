//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define LMC_WIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define LMC_LINUX
#endif

#if defined(LMC_WIN)
#	ifdef LMC_API_STATIC
#		ifdef __cplusplus
#			define LMC_API extern "C" inline
#		else
#			define LMC_API inline
#		endif
#	else
#		ifdef LMC_API_EXPORTS
#			ifdef __cplusplus
#				define LMC_API extern "C" __declspec(dllexport) inline
#			else
#				define LMC_API __declspec(dllexport) inline
#			endif
#		else
#			ifdef __cplusplus
#				define LMC_API extern "C" __declspec(dllimport) inline
#			else
#				define LMC_API __declspec(dllimport) inline
#			endif
#		endif
#	endif
#elif defined(LMC_LINUX)
#	ifdef __cplusplus
#		define MY_API extern "C" inline
#	else
#		define MY_API inline
#	endif
#endif

#define LMC_GVariable(name) __G_##name##_

/*
 *
 * Type defines
 *
 * */
using FloatTy = float;
using TimerTy = std::chrono::high_resolution_clock;