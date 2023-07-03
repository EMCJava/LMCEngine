//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"

class EditorWindow : public Window
{
public:
	/*
	 *
	 * Create a normal window.
	 *
	 * */
	EditorWindow(int Width, int Height, const char *Title, bool Fullscreen = false, bool Create = true);

	/*
	 *
	 * Create a fullscreen window, with full resolution.
	 *
	 * */
	EditorWindow(const char *Title, bool Create = true);

	~EditorWindow();

	/*
	 *
	 * Render the editor layout
	 *
	 * */
	void
	Update() override;

	class Project *
	GetProject() const;

private:
	class Project *m_ActiveProject = nullptr;
};
