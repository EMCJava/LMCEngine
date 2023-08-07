//
// Created by loyus on 7/12/2023.
//

#pragma once

#include <spdlog/spdlog.h>

#ifndef NDEBUG

/*
 *
 * Test is for debug only, and not vital
 *
 * */
#    define TEST( x )                                                           \
        if ( !( x ) ) [[unlikely]]                                              \
        {                                                                       \
            spdlog::warn( "Test failed: {} at {}:{}", #x, __FILE__, __LINE__ ); \
        }
#else

/*
 *
 * Test is for debug only, and not vital
 *
 * */
#    define TEST( x ) ( (void) 0 );

#endif

/*
 *
 * Required is for both debug and release, which is vital
 *
 * */
#define REQUIRED( x, ... )                                                          \
    if ( !( x ) ) [[unlikely]]                                                      \
    {                                                                               \
        spdlog::critical( "Required failed: {} at {}:{}", #x, __FILE__, __LINE__ ); \
        __VA_ARGS__;                                                                \
    }

/*
 *
 * Required is for both debug and release, which is vital
 *
 * */
#define REQUIRED_IF( x, ... )                                                       \
    if ( !( x ) ) [[unlikely]]                                                      \
    {                                                                               \
        spdlog::critical( "Required failed: {} at {}:{}", #x, __FILE__, __LINE__ ); \
        __VA_ARGS__;                                                                \
    } else
