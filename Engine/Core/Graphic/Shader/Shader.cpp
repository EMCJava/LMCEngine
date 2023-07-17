//
// Created by loyus on 7/16/2023.
//

#include "Shader.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

#include <fstream>

void
Shader::Load(const char *Vertex, const char *Fragment)
{
	const auto *gl = Engine::GetEngine()->GetGLContext();
	GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = gl->CreateShader(GL_VERTEX_SHADER);
	gl->ShaderSource(vertexShader, 1, &Vertex, nullptr);
	gl->CompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	gl->GetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		gl->GetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		spdlog::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED {}", infoLog);
		abort();
	}

	// fragment shader
	unsigned int fragmentShader = gl->CreateShader(GL_FRAGMENT_SHADER);
	gl->ShaderSource(fragmentShader, 1, &Fragment, nullptr);
	gl->CompileShader(fragmentShader);

	// check for shader compile errors
	gl->GetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		gl->GetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED {}", infoLog);
		abort();
	}

	// link shaders
	m_ProgramID = gl->CreateProgram();
	gl->AttachShader(m_ProgramID, vertexShader);
	gl->AttachShader(m_ProgramID, fragmentShader);
	gl->LinkProgram(m_ProgramID);
	// check for linking errors
	gl->GetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		gl->GetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
		spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED {}", infoLog);
		abort();
	}
	gl->DeleteShader(vertexShader);
	gl->DeleteShader(fragmentShader);
}

void
Shader::Bind() const
{
	if (m_ProgramID != 0)
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();
		GL_CHECK(gl->UseProgram(m_ProgramID))
	}
}

Shader::~Shader()
{
	const auto *gl = Engine::GetEngine()->GetGLContext();

	if (m_ProgramID != 0)
	{
		GL_CHECK(gl->DeleteProgram(m_ProgramID))
		m_ProgramID = 0;
	}
}

void
Shader::LoadFromFile(std::string_view VertexPath, std::string_view FragmentPath)
{
	std::ifstream VertexShaderFile;
	std::ifstream FragmentShaderFile;

	std::string VertexShaderFileContent{std::istreambuf_iterator<char>(VertexShaderFile), std::istreambuf_iterator<char>()};
	std::string FragmentShaderFileContent{std::istreambuf_iterator<char>(FragmentShaderFile), std::istreambuf_iterator<char>()};

	Load(VertexShaderFileContent.c_str(), FragmentShaderFileContent.c_str());
}

int
Shader::GetUniformLocation(const std::string &Name) const
{
	const auto *gl = Engine::GetEngine()->GetGLContext();
	return gl->GetUniformLocation(m_ProgramID, Name.c_str());
}

void
Shader::SetMat4(const std::string &Name, const glm::mat4 &mat) const
{
	const auto *gl = Engine::GetEngine()->GetGLContext();
	gl->UniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, &mat[0][0]);
}
