//
// Created by loyus on 7/12/2023.
//

#pragma once

#include "Sprite.hpp"

class SpriteSquare : public Sprite
{
	DECLARE_CONCEPT(SpriteSquare, Sprite)
public:
	SpriteSquare();

	virtual void
	Render();
};
