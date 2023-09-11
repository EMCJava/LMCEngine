//
// Created by samsa on 9/10/2023.
//

#include "Font.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/API/OpenGL.hpp>
#include <Engine/Core/Runtime/Assertion/Assertion.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H

#include <spdlog/spdlog.h>

/*
void A(){
    QString familyName(face->family_name);

    QVector<FT_SfntName> names;

    for (FT_UInt i = 0; i < FT_Get_Sfnt_Name_Count(face); i++)
    {
        FT_SfntName name;
        if (!FT_Get_Sfnt_Name(face, i, &name))
        {
            switch (name.name_id) {
            case TT_NAME_ID_FONT_FAMILY:
            case TT_NAME_ID_PREFERRED_FAMILY:
                if (name.platform_id != TT_PLATFORM_MACINTOSH)
                    names.append(name);
                break;
            default:
                break;
            }
        }
    }

    if (!names.isEmpty())
    {
        std::sort(names.begin(), names.end(), nameComp);
        foreach (const FT_SfntName& name, names)
        {
            QString string(decodeNameRecord(name));
            if (!string.isEmpty())
            {
                familyName = string;
                break;
            }
        }
    }

    return familyName;
}
*/

Font::Character*
Font::GetCharacter( char Char )
{
    if ( m_Characters.contains( Char ) )
    {
        return &m_Characters[ Char ];
    }

    return nullptr;
}

void
Font::LoadFont( const std::string& Path )
{
    auto DefaultFont = std::make_shared<Font>( );

    FT_Library ft;
    REQUIRED_IF( !FT_Init_FreeType( &ft ) )
    {
        FT_Face face;
        REQUIRED_IF( !FT_New_Face( ft, Path.c_str( ), 0, &face ) )
        {
            DefaultFont->m_FontName = face->family_name;

            FT_Set_Pixel_Sizes( face, 0, 48 );

            REQUIRED_IF( !FT_Load_Char( face, 'X', FT_LOAD_RENDER ) )
            {
                const auto* gl = Engine::GetEngine( )->GetGLContext( );
                gl->PixelStorei( GL_UNPACK_ALIGNMENT, 1 );   // disable byte-alignment restriction

                for ( unsigned char c = 0; c < 128; c++ )
                {
                    // load character glyph
                    REQUIRED_IF( !FT_Load_Char( face, c, FT_LOAD_RENDER ) )
                    {
                        // generate texture
                        unsigned int texture;
                        gl->GenTextures( 1, &texture );
                        gl->BindTexture( GL_TEXTURE_2D, texture );
                        gl->TexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            GL_RED,
                            face->glyph->bitmap.width,
                            face->glyph->bitmap.rows,
                            0,
                            GL_RED,
                            GL_UNSIGNED_BYTE,
                            face->glyph->bitmap.buffer );
                        // set texture options
                        gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                        gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                        gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                        gl->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                        // now store character for later use

                        Font::Character character = {
                            texture,
                            std::make_pair( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
                            std::make_pair( face->glyph->bitmap_left, face->glyph->bitmap_top ),
                            face->glyph->advance.x };
                        DefaultFont->m_Characters.insert( std::pair<char, Font::Character>( c, character ) );
                    }
                }
            }

            FT_Done_Face( face );
        }

        FT_Done_FreeType( ft );
    }
}