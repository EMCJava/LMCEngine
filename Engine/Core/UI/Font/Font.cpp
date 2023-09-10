//
// Created by samsa on 9/10/2023.
//

#include "Font.hpp"

Font::Character*
Font::GetCharacter( char Char )
{
    if ( m_Characters.contains( Char ) )
    {
        return &m_Characters[ Char ];
    }

    return nullptr;
}
