//
// Created by loyus on 7/1/2023.
//

#include <Engine/Core/Window/Window.hpp>

#pragma once

class Engine
{
public:
	Engine();
	~Engine();

	/*
	 *
	 * Engine's main update loop
	 *
	 * */
	void
	Update();

	/*
	 *
	 * Return whether the engine should be in a update loop
	 *
	 * */
	bool
	ShouldShutdown();

private:
	bool m_ShouldShutdown = false;

	Window *m_MainWindow = nullptr;
};