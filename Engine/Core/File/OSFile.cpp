//
// Created by loyus on 7/2/2023.
//

#include "OSFile.hpp"

#include <stdexcept>

std::string
OSFile::PickFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path )
{
    nfdchar_t*  outPath = nullptr;
    nfdresult_t result  = NFD::OpenDialog( outPath, filter.data( ), filter.size( ), default_path );

    if ( result == NFD_OKAY )
    {
        std::string path( outPath );
        free( outPath );

        return path;
    } else if ( result == NFD_CANCEL )
    {
        return "";
    } else
    {
        throw std::runtime_error( std::string( "OSFile::PickFile error: " ) + NFD_GetError( ) );
    }
}

std::string
OSFile::SaveFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path )
{
    nfdchar_t*  outPath = nullptr;
    nfdresult_t result  = NFD::OpenDialog( outPath, filter.data( ), filter.size( ), default_path );

    if ( result == NFD_OKAY )
    {
        std::string path( outPath );
        free( outPath );

        return path;
    } else if ( result == NFD_CANCEL )
    {
        return "";
    } else
    {
        throw std::runtime_error( std::string( "OSFile::PickFile error: " ) + NFD_GetError( ) );
    }
}
