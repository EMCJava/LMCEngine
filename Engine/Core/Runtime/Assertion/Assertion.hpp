//
// Created by loyus on 7/12/2023.
//

#pragma once

#include <spdlog/spdlog.h>

#include <source_location>

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
#define REQUIRED( x, ... )                                                                                                             \
    if ( !( x ) ) [[unlikely]]                                                                                                         \
    {                                                                                                                                  \
        const auto SL = std::source_location::current( );                                                                              \
        spdlog::critical( "Required failed: {} at {}({}:{}) `{}`", #x, SL.file_name( ), SL.line( ), SL.line( ), SL.function_name( ) ); \
        __VA_ARGS__;                                                                                                                   \
    }

/*
 *
 * Required is for both debug and release, which is vital
 *
 * */
#define REQUIRED_IF( x, ... )                                                       \
    if ( !( x ) ) [[unlikely]]                                                      \
    {                                                                               \
        const auto SL = std::source_location::current( );                                                                              \
        spdlog::critical( "Required failed: {} at {}({}:{}) `{}`", #x, SL.file_name( ), SL.line( ), SL.line( ), SL.function_name( ) ); \
        __VA_ARGS__;                                                                \
    } else
