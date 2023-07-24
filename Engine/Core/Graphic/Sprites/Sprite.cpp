//
// Created by loyus on 7/16/2023.
//

#include "Sprite.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

DEFINE_CONCEPT(Sprite, ConceptRenderable)

Sprite::~Sprite()
{
	const auto *gl = Engine::GetEngine()->GetGLContext();
	GL_CHECK(Engine::GetEngine()->MakeMainWindowCurrentContext())

	GL_CHECK(gl->DeleteVertexArrays(1, &m_VAO));
	GL_CHECK(gl->DeleteBuffers(1, &m_VBO));
	GL_CHECK(gl->DeleteBuffers(1, &m_EBO));

	m_VAO = m_VBO = m_EBO = 0;
}

void
Sprite::Render()
{
	if (m_Shader != nullptr)
	{
		const auto Dimensions = Engine::GetEngine()->GetMainWindowViewPortDimensions();

		m_Shader->Bind();
		auto Projection = glm::ortho<float>(0, Dimensions.first, 0, Dimensions.second, -1, 1);
		m_Shader->SetMat4("projectionMatrix", Projection);

		auto Model = glm::translate(glm::mat4(1), glm::vec3(m_Coordinate.X, m_Coordinate.Y, 0));
		m_Shader->SetMat4("modelMatrix", Model);

	}
}

void
Sprite::SetShader(const std::shared_ptr<Shader> &shader)
{
	m_Shader = shader;
}
