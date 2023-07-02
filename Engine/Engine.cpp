//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <Engine/Core/Environment/Environment.hpp>
#include <Engine/Core/Window/EditorWindow.hpp>
#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Window/WindowPool.hpp>

#include <iostream>

Engine *Engine::g_Engine = nullptr;

Engine::Engine()
{
	if (g_Engine != nullptr)
	{
		throw std::runtime_error("There is already a Engine instance!");
	}

	spdlog::info("Engine initializing");
	InitializeEnvironment();

	bool Fullscreen = false;
	m_MainWindow = new EditorWindow(1280, 720, "LMCEngine");

	/*
	 *
	 * Initialize GLAD & ImGui
	 *
	 * */

	[[maybe_unused]] int version = gladLoaderLoadGL();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	m_ImGuiContext = ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;
	if (!Fullscreen)
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	io.IniFilename = nullptr;
	ImGui::LoadIniSettingsFromDisk("imgui.ini");

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_MainWindow->GetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init();

	m_MainWindowPool = new WindowPool;
	m_MainWindowPool->CreateWindow<GameWindow>(1280, 720, "Game Viewer");

	g_Engine = this;
}

Engine::~Engine()
{
	spdlog::info("Engine destroying");

	delete m_MainWindow;
	m_MainWindow = nullptr;

	if (m_ImGuiContext != nullptr)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_ImGuiContext);
		m_ImGuiContext = nullptr;
	}

	delete m_MainWindowPool;
	m_MainWindowPool = nullptr;

	ShutdownEnvironment();
}

void
Engine::Update()
{
	m_MainWindow->MakeContextCurrent();

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_MainWindow->Update();
	m_MainWindowPool->Update();

	m_MainWindow->MakeContextCurrent();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	glfwSwapBuffers(m_MainWindow->GetWindowHandle());

	m_ShouldShutdown |= m_MainWindow->WindowShouldClose();
}

bool
Engine::ShouldShutdown()
{
	return m_ShouldShutdown;
}
