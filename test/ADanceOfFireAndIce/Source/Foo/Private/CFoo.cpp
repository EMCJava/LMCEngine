//
// Created by samsa on 7/8/2023.
//

#include "CFoo.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Concept/ConceptCoordinate.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT_MA_SE(CFoo, Concept)

class Sprite : public ConceptRenderable
{
	DECLARE_CONCEPT(Sprite, ConceptRenderable)

	const char *vertexShaderSource = "#version 330 core\n"
	                                 "layout (location = 0) in vec3 aPos;\n"
	                                 "void main()\n"
	                                 "{\n"
	                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	                                 "}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
	                                   "out vec4 FragColor;\n"
	                                   "void main()\n"
	                                   "{\n"
	                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	                                   "}\n\0";

	void
	SetupShader()
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();
		GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

		// build and compile our shader program
		// ------------------------------------
		// vertex shader
		unsigned int vertexShader = gl->CreateShader(GL_VERTEX_SHADER);
		gl->ShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
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
		gl->ShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
		gl->CompileShader(fragmentShader);
		// check for shader compile errors
		gl->GetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			gl->GetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
			spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED {}", infoLog);
		}
		// link shaders
		m_ShaderProgram = gl->CreateProgram();
		gl->AttachShader(m_ShaderProgram, vertexShader);
		gl->AttachShader(m_ShaderProgram, fragmentShader);
		gl->LinkProgram(m_ShaderProgram);
		// check for linking errors
		gl->GetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			gl->GetProgramInfoLog(m_ShaderProgram, 512, nullptr, infoLog);
			spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED {}", infoLog);
		}
		gl->DeleteShader(vertexShader);
		gl->DeleteShader(fragmentShader);
	}

	void
	SetupRenderBuffer()
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();
		GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

		float vertices[] = {
		    0.5f, 0.5f, 0.0f,  // top right
		    0.5f, -0.5f, 0.0f, // bottom right
		    -0.5f, -0.5f, 0.0f,// bottom left
		    -0.5f, 0.5f, 0.0f  // top left
		};
		unsigned int indices[] = {
		    // note that we start from 0!
		    0, 1, 3,// first triangle
		    1, 2, 3 // second triangle
		};

		GL_CHECK(gl->GenVertexArrays(1, &m_VAO))
		GL_CHECK(gl->BindVertexArray(m_VAO))

		// 2. copy our vertices array in a buffer for OpenGL to use
		GL_CHECK(gl->GenBuffers(1, &m_VBO))
		GL_CHECK(gl->BindBuffer(GL_ARRAY_BUFFER, m_VBO))
		GL_CHECK(gl->BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW))

		// 3. then set our vertex attributes pointers
		GL_CHECK(gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr))
		GL_CHECK(gl->EnableVertexAttribArray(0))

		GL_CHECK(gl->GenBuffers(1, &m_EBO))
		GL_CHECK(gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO))
		GL_CHECK(gl->BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW))
	}

public:
	Sprite()
	{
		SetupShader();
		SetupRenderBuffer();
	}

	~Sprite()
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();
		GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

		gl->DeleteVertexArrays(1, &m_VAO);
		gl->DeleteBuffers(1, &m_VBO);
		gl->DeleteBuffers(1, &m_EBO);
		gl->DeleteProgram(m_ShaderProgram);

		m_VAO = m_VBO = m_EBO = m_ShaderProgram = 0;
	}

	void
	Render() override
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();

		GL_CHECK(gl->UseProgram(m_ShaderProgram))
		GL_CHECK(gl->BindVertexArray(m_VAO))
		GL_CHECK(gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO))
		GL_CHECK(gl->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr))
	}

private:
	uint32_t m_VAO{}, m_VBO{}, m_EBO{}, m_ShaderProgram{};
};
DEFINE_CONCEPT(Sprite, ConceptRenderable)

CFoo::CFoo()
{
	spdlog::info("CFoo concept constructor called");

	AddConcept<ConceptCoordinate>()->GetCoordinate();
	AddConcept<Sprite>();

	spdlog::info("CFoo concept constructor returned");
}

CFoo::~CFoo()
{
	spdlog::info("CFoo concept destructor called");
}

void
CFoo::Apply()
{
	//	spdlog::info("CFoo concept Apply() called, DeltaTime: {}, Coordinate: {}",
	//	             Engine::GetEngine()->GetDeltaSecond(),
	//	             GetConcept<ConceptCoordinate>()->GetCoordinate());
}
