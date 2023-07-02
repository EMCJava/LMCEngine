//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

class GameWindow : public Window
{
public:
	using Window::Window;

	void
	Update() override;
};
