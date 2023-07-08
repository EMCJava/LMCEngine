//
// Created by loyus on 7/2/2023.
//

#pragma once

#include "Window.hpp"
#include "Engine/Core/Concept/Concept.hpp"

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

	/*
	 *
	 * Root concept in the screen
	 *
	 * */
	std::unique_ptr<Concept> RootConcept;

	class Project *m_ActiveProject = nullptr;
};
