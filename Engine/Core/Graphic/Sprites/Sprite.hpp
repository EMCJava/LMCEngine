//
// Created by loyus on 7/16/2023.
//

#pragma once

#include <Engine/Core/Concept/PureConceptCoordinate.hpp>
#include <Engine/Core/Concept/ConceptRenderable.hpp>
#include <Engine/Core/Graphic/Shader/Shader.hpp>

#include <memory>

class Sprite : public ConceptRenderable, public PureConceptCoordinate
{
	DECLARE_CONCEPT(Sprite, ConceptRenderable)

public:
	Sprite() = default;

	void
	Render() override;

	void
	SetShader(const std::shared_ptr<Shader> &shader);

	/*
	 *
	 * Setup/Recreate sprite
	 * Call when sprite changes, e.g. geometry changes, size change, window resize
	 *
	 * */
	virtual void
	SetupSprite() = 0;

protected:
	std::shared_ptr<Shader> m_Shader;
	uint32_t m_VAO{}, m_VBO{}, m_EBO{};
};