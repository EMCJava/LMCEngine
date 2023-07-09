//
// Created by loyus on 7/1/2023.
//

#pragma once

#include <string>

using PrimoryWindow = class EditorWindow;

class Engine
{
	/*
	 *
	 * Engine's render job
	 *
	 * */
	void
	Render();

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
	 * Load project, this has to be called from engine
	 * because it will initialize the concept
	 *
	 * */
	void
	LoadProject(const std::string &Path);

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
	 * Root concept in the screen
	 *
	 * */
	class DynamicConcept *m_RootConcept = nullptr;

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