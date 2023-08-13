//
// Created by Lo Yu Sum on 27/7/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConcept.hpp>

#include <string_view>

class ShaderProgram : public PureConcept
{
    DECLARE_CONCEPT( ShaderProgram, PureConcept )
public:
    ShaderProgram( ) = default;

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

    ENABLE_IMGUI( ShaderProgram )
};
