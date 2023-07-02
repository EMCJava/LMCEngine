//
// Created by loyus on 7/1/2023.
//

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

	class WindowPool *m_MainWindowPool = nullptr;

	/*
	 *
	 * Global instance of the engine
	 *
	 * */
	static Engine *g_Engine;
};