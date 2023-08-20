//
// Created by samsa on 8/20/2023.
//

#pragma once

#include <chrono>

class StopWatch
{

public:
    StopWatch( bool StartTimer = true, bool LogOnDestruct = true )
        : m_LogOnDestruct( LogOnDestruct )
    {
        if ( StartTimer )
        {
            Start( );
        }
    }

    ~StopWatch( )
    {
        if ( m_LogOnDestruct )
        {
            Stop( );
            spdlog::info( "StopWatch: {}s", GetSecondsElapsed( ) );
        }
    }

    static std::chrono::high_resolution_clock::time_point
    Now( )
    {
        return std::chrono::high_resolution_clock::now( );
    }

    void Start( )
    {
        m_Start = Now( );
    }

    void Stop( )
    {
        m_Stop = Now( );
    }

    FloatTy
    GetSecondsElapsed( ) const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>( m_Stop - m_Start ).count( ) / 1000.F;
    }

private:
    std::chrono::high_resolution_clock::time_point m_Start;
    std::chrono::high_resolution_clock::time_point m_Stop;
    bool                                           m_LogOnDestruct;
};