//
// Created by Lo Yu Sum on 27/7/2023.
//

#pragma once

#include <string_view>

class ShaderProgram
{
public:
    ShaderProgram( ) = default;
    ~ShaderProgram( );

    void
    Load( const char* Vertex, const char* Fragment );

    void
    LoadFromFile( std::string_view VertexPath, std::string_view FragmentPath );

    void
    Bind( ) const;

    uint32_t
    GetProgramID( ) const;

protected:
    uint32_t m_ProgramID = 0;
};
