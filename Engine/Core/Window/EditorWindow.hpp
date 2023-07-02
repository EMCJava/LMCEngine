//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

class EditorWindow : public Window
{
public:
	using Window::Window;

	/*
	 *
	 * Render the editor layout
	 *
	 * */
	void Update() override;
};
