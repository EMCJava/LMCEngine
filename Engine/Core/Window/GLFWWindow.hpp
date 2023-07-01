//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <string>

class Window
{
public:
	Window(int Width, int Height, const char *Title, bool Fullscreen = false, bool Create = true);
	~Window();

	/*
	 *
	 * Take all the config and create the window.
	 *
	 * */
	void
	CreateWindow();

	/*
	 *
	 * The main loop of the window, polling events etc.
	 *
	 * */
	void
	Update();

	/*
     *
     * Set swap interval for this window
     * Thus this function makes call to MakeContextCurrent
     *
     * */
	void
	SetSwapInterval(int interval);

	/*
	 *
	 * Make all calls to glfw focus this window.
	 *
	 * */
	void
	MakeContextCurrent();

	/*
	 *
	 * Return whether user requested to close the window.
	 *
	 * */
	bool
	WindowShouldClose();

private:
	int m_Width, m_Height;
	bool m_Fullscreen;
	std::string m_Title;

	class GLFWwindow *m_Window = nullptr;
};
