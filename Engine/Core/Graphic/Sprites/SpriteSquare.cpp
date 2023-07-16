//
// Created by loyus on 7/12/2023.
//

#include "SpriteSquare.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT(SpriteSquare, Sprite)

SpriteSquare::SpriteSquare()
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

void
SpriteSquare::Render()
{
	Sprite::Render();

	const auto *gl = Engine::GetEngine()->GetGLContext();

	GL_CHECK(gl->BindVertexArray(m_VAO))
	GL_CHECK(gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO))
	GL_CHECK(gl->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr))
}
