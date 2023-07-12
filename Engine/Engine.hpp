//
// Created by loyus on 7/1/2023.
//

#pragma once

#include <Engine/Core/Core.hpp>

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
	 * This will also invoke UpdateRootConcept
	 *
	 * */
	void
	Update();

	/*
	 *
	 * Engine's main update loop
	 *
	 * */
	void
	UpdateRootConcept();

	/*
	 *
	 * Return whether the engine should be in a update loop
	 *
	 * */
	bool
	ShouldShutdown();

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

	static Engine *
	GetEngine();

	/*
	 *
	 * Set the global engine context
	 * This can be useful when dynamic libraries are used
	 *
	 * */
	static void
	SetEngine(Engine *EngineContext);

	/*
	 *
	 * Time related
	 *
	 * */
	void
	ResetTimer();

	FloatTy
	GetDeltaSecond() const;

private:
	bool m_ShouldShutdown = false;

	/*
	 *
	 * Time related
	 *
	 * */
	TimerTy::time_point m_FirstUpdateTime;
	TimerTy::time_point m_LastUpdateTime;
	TimerTy::time_point m_CurrentUpdateTime;
	// This will be set at the start of the update loop
	FloatTy m_DeltaSecond = 0.0f;

	/*
	 *
	 * Windows
	 *
	 * */
	PrimoryWindow *m_MainWindow = nullptr;
	class WindowPool *m_MainWindowPool = nullptr;

	/*
	 *
	 * ImGui
	 *
	 * */
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