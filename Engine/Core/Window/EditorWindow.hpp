//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

class EditorWindow : public Window
{
public:
	using Window::Window;

	~EditorWindow();

	/*
	 *
	 * Render the editor layout
	 *
	 * */
	void
	Update() override;

private:
	class Project *m_ActiveProject = nullptr;
};
