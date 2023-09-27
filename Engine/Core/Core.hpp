//
// Created by loyus on 7/4/2023.
//

#pragma once

#include <chrono>

/*
 *
 * Platform macros
 *
 * */
#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __NT__ )
// define something for Windows (32-bit and 64-bit, this part is common)
#    define LMC_WIN
#    ifdef _WIN64
// define something for Windows (64-bit only)
#    else
// define something for Windows (32-bit only)
#    endif
#elif __APPLE__
#    include <TargetConditionals.h>
#    if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#    elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#    elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#    elif TARGET_OS_MAC
#        define LMC_APPLE
#    else
#        error "Unknown Apple platform"
#    endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#elif __linux__
// linux
#    define LMC_LINUX
#elif __unix__   // all unices not caught above
// Unix
#elif defined( _POSIX_VERSION )
// POSIX
#else
#    error "Unknown compiler"
#endif

/*
 *
 * Export macros
 *
 * */
#if defined( LMC_WIN )
#    ifdef LMC_API_STATIC
#        ifdef __cplusplus
#            define LMC_API extern "C" inline
#        else
#            define LMC_API inline
#        endif
#    else
#        ifdef LMC_API_EXPORTS
#            ifdef __cplusplus
#                define LMC_API extern "C" __declspec( dllexport ) inline
#            else
#                define LMC_API __declspec( dllexport ) inline
#            endif
#        else
#            ifdef __cplusplus
#                define LMC_API extern "C" __declspec( dllimport ) inline
#            else
#                define LMC_API __declspec( dllimport ) inline
#            endif
#        endif
#    endif
#elif defined( LMC_LINUX )
#    ifdef __cplusplus
#        define LMC_API extern "C" __attribute__((visibility("default")))
#    else
#        define LMC_API
#    endif
#elif defined( LMC_APPLE )
#    ifdef __cplusplus
#        define LMC_API extern "C" __attribute__( ( visibility( "default" ) ) )
#    else
#        define LMC_API
#    endif
#endif

#define LMC_GVariable( name ) __G_##name##_

#ifdef _MSC_VER
#    define LMC_INIT_PRIORITY( priority )
#else
#    define LMC_INIT_PRIORITY( priority ) __attribute__( ( init_priority( priority ) ) )
#endif

/*
 *
 * Type defines
 *
 * */
using FloatTy = float;
using TimerTy = std::chrono::high_resolution_clock;