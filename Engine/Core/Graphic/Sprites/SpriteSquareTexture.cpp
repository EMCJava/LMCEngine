//
// Created by loyus on 23/7/2023.
//

#include "SpriteSquareTexture.hpp"

#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Engine.hpp>

#include <stb_image.h>

DEFINE_CONCEPT_DS(SpriteSquareTexture, SpriteSquare)

void
SpriteSquareTexture::SetupSprite()
{
	spdlog::info("Setting up texture {}", m_TexturePath);

	const auto *gl = Engine::GetEngine()->GetGLContext();
	GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

	float vertices[] = {
	    // positions   // texture coords
	    1.f, 1.f, 0.f, 1.0f, 1.0f,// top right
	    1.f, 0.f, 0.f, 1.0f, 0.0f,// bottom right
	    0.f, 0.f, 0.f, 0.0f, 0.0f,// bottom left
	    0.f, 1.f, 0.f, 0.0f, 1.0f // top left
	};

	for (int i = 0; i < std::size(vertices); i += 5)
	{
		vertices[i + 0] *= m_Width;
		vertices[i + 1] *= m_Height;
	}

	unsigned int indices[] = {
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
	GL_CHECK(gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr))
	GL_CHECK(gl->EnableVertexAttribArray(0))

	GL_CHECK(gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))))
	GL_CHECK(gl->EnableVertexAttribArray(1))

	GL_CHECK(gl->GenBuffers(1, &m_EBO))
	GL_CHECK(gl->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO))
	GL_CHECK(gl->BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW))

	// Setup texture

	// texture 1
	// ---------
	gl->GenTextures(1, &m_TextureID);
	gl->BindTexture(GL_TEXTURE_2D, m_TextureID);
	// set the texture wrapping parameters
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);// tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(m_TexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		gl->GenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		spdlog::error("Failed to load texture");
	}
	stbi_image_free(data);

	m_Shader->Bind();
	gl->Uniform1i(m_Shader->GetUniformLocation("sample_texture"), 0);
}

void
SpriteSquareTexture::SetTexturePath(const std::string &TexturePath)
{
	m_TexturePath = TexturePath;
}

void
SpriteSquareTexture::Render()
{
	if (m_Shader != nullptr)
	{
		const auto *gl = Engine::GetEngine()->GetGLContext();
		gl->ActiveTexture(GL_TEXTURE0);
		gl->BindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	SpriteSquare::Render();
}
