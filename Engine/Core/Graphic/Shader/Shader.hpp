//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <glm/glm.hpp>

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

	/*
	 *
	 * Shader operations
	 *
	 * */
	int
	GetUniformLocation(const std::string &Name) const;

	void SetMat4(const std::string &Name, const glm::mat4 &mat) const;

private:
	uint32_t m_ProgramID = 0;
};
