//
// Created by loyus on 7/1/2023.
//

#pragma once

using PrimoryWindow = class EditorWindow;

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

	static Engine *
	GetEngine();

	/*
	 *
	 * current opened project
	 *
	 * */
	class Project *
	GetProject() const;

private:
	bool m_ShouldShutdown = false;

	PrimoryWindow *m_MainWindow = nullptr;
	class WindowPool *m_MainWindowPool = nullptr;

	struct ImGuiContext *m_ImGuiContext = nullptr;

	/*
	 *
	 * Should only be call after m_MainWindow has been initialized
	 *
	 * */
	void
	CreateImGuiContext();
	void
	DestroyImGuiContext();

	/*
	 *
	 * Active instance of current opened project
	 *
	 * */
	class Project *m_ActiveProject = nullptr;

	/*
	 *
	 * Global instance of the engine
	 *
	 * */
	static Engine *g_Engine;
};