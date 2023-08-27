//
// Created by loyus on 8/27/2023.
//

#pragma once

#include <libexecstream/exec-stream.h>

#include <thread>
#include <spdlog/spdlog.h>

namespace ExternalProgram
{

inline int
RunCommand( const std::string& Program, const std::string& Arguments, auto&& OnStdOut, auto&& OnStdErr, const std::vector<std::pair<std::string, std::string>>& PreCommands = { } )
{
    exec_stream_t es;
    es.set_wait_timeout( exec_stream_t::stream_kind_t::s_all, 10 * 60 * 1000 );

    for ( const auto& Cmd : PreCommands )
    {
        spdlog::info( "Running command: {} {}", Cmd.first, Cmd.second );
        es.start( Cmd.first, Cmd.second );
        std::string s;
        while ( std::getline( es.out( ), s ) )
        {
            spdlog::info( "exec_stream(out) : - {} -", s );
        }
        while ( std::getline( es.err( ), s ) )
        {
            spdlog::info( "exec_stream(err) : - {} -", s );
        }
        es.close( );
    }

    spdlog::info( "Running command: {} {}", Program, Arguments );
    es.start( Program, Arguments );

    auto StdErr = std::thread( [ &es, &OnStdErr ]( ) {
        std::string s;
        while ( std::getline( es.err( ), s ) )
        {
            OnStdErr( s );
        }
    } );

    auto StdOut = std::thread( [ &es, &OnStdOut ]( ) {
        std::string s;
        while ( std::getline( es.out( ), s ) )
        {
            OnStdOut( s );
        }
    } );

    if ( StdErr.joinable( ) ) StdErr.join( );
    if ( StdOut.joinable( ) ) StdOut.join( );

    es.close( );
    return es.exit_code( );
}
}   // namespace ExternalProgram