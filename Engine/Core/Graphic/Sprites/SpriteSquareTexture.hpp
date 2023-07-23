//
// Created by loyus on 23/7/2023.
//

#pragma once

#include "SpriteSquare.hpp"

class SpriteSquareTexture : public SpriteSquare
{
	DECLARE_CONCEPT(SpriteSquareTexture, SpriteSquare)

public:
	using SpriteSquare::SpriteSquare;

	void
	SetTexturePath(const std::string &TexturePath);

	void
	Render() override;

	void
	SetupSprite() override;

protected:
	uint32_t m_TextureID;
	std::string m_TexturePath;
};
