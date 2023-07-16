//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <cstdint>

class Shader
{
public:
	Shader() = default;
	~Shader();

	void
	Load(const char *Vertex, const char *Fragment);

	void
	Bind() const;

private:

	uint32_t m_ProgramID = 0;
};
