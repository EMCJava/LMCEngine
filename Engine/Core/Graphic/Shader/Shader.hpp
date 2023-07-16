//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <string>
#include <cstdint>

class Shader
{
public:
	Shader() = default;
	~Shader();

	void
	Load(const char *Vertex, const char *Fragment);

	void
	LoadFromFile(std::string_view VertexPath, std::string_view FragmentPath);

	void
	Bind() const;

private:

	uint32_t m_ProgramID = 0;
};
