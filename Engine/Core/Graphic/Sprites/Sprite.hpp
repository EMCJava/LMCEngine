//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <memory>

class Sprite : public ConceptRenderable
{
	DECLARE_CONCEPT(Sprite, ConceptRenderable)

public:

	Sprite() = default;
	~Sprite();

	void
	Render() override;

	void
	SetShader(const std::shared_ptr<Shader> &shader);

protected:
	std::shared_ptr<Shader> m_Shader;
	uint32_t m_VAO{}, m_VBO{}, m_EBO{};
};
