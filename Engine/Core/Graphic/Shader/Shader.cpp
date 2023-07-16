//
// Created by loyus on 7/16/2023.
//

#include "Shader.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <spdlog/spdlog.h>

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
	}
	gl->DeleteShader(vertexShader);
	gl->DeleteShader(fragmentShader);
}

void
Shader::Bind() const
{
	const auto *gl = Engine::GetEngine()->GetGLContext();
	GL_CHECK(gl->UseProgram(m_ProgramID))
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
