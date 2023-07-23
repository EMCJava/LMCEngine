//
// Created by loyus on 7/12/2023.
//

#pragma once

#include "Sprite.hpp"

class SpriteSquare : public Sprite
{
	DECLARE_CONCEPT(SpriteSquare, Sprite)

public:
	SpriteSquare(int Width, int Height);

	virtual void
	Render();

	void
	SetupSprite();

private:
	int m_Width = 0, m_Height = 0;
};
