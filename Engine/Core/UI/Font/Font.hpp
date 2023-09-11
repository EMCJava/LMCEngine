//
// Created by samsa on 9/10/2023.
//

#pragma once

#include <string>
#include <map>

class Font
{
public:
    struct Character {
        unsigned int        TextureID { };   // ID handle of the glyph texture
        std::pair<int, int> Size { };        // Size of glyph
        std::pair<int, int> Bearing { };     // Offset from baseline to left/top of glyph
        long                Advance { };     // Offset to advance to next glyph
    };

    void
    LoadFont( const std::string& Path );

    /*
     *
     * return nullptr if Char not exist
     *
     * */
    Character*
    GetCharacter( char Char );

protected:
    std::string               m_FontName;
    std::map<char, Character> m_Characters;

    friend class Engine;
};