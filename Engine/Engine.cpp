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

#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>
#include <Engine/Core/Environment/Environment.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>
#include <Engine/Core/Window/EditorWindow.hpp>
#include <Engine/Core/Window/GameWindow.hpp>
#include <Engine/Core/Window/WindowPool.hpp>
#include <Engine/Core/Project/Project.hpp>

#include <format>

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
	CreateImGuiContext();

	m_MainWindowPool = new WindowPool;
	// m_MainWindowPool->CreateWindow<GameWindow>(1280, 720, "Game Viewer");

	/*
	 *
	 * Engine
	 *
	 * */
	m_ActiveProject = new Project;

	g_Engine = this;
}

Engine::~Engine()
{
	spdlog::info("Engine destroying");

	delete m_RootConcept;
	m_RootConcept = nullptr;

	delete m_ActiveProject;
	m_ActiveProject = nullptr;

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
	if (m_RootConcept != nullptr)
	{
		auto &RootConcept = *m_RootConcept;

		if (RootConcept.ShouldReload())
		{
			spdlog::info("RootConcept changes detected, hot reloading");
			RootConcept.Reload();
		}

		RootConcept->Apply();
	}

	Render();

	m_ShouldShutdown |= m_MainWindow->WindowShouldClose();
}

void
Engine::Render()
{
	m_MainWindow->MakeContextCurrent();

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	try
	{
		m_MainWindow->Update();
		m_MainWindowPool->Update();
	}
	catch (const ImGuiContextInvalid &e)
	{
		CreateImGuiContext();
		return;
	}

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
}

bool
Engine::ShouldShutdown()
{
	return m_ShouldShutdown;
}

void
Engine::CreateImGuiContext()
{
	IMGUI_CHECKVERSION();
	if (m_ImGuiContext != nullptr)
	{
		DestroyImGuiContext();
	}
	m_ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;
	if (!m_MainWindow->IsFullscreen())
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	io.IniFilename = nullptr;

	std::string ConfigLayout;
	if (m_ActiveProject != nullptr)
	{
		ConfigLayout = m_ActiveProject->GetConfig().editor_layout_path;
		if (!ConfigLayout.empty())
		{
			spdlog::info("Loading layout: {}", ConfigLayout);
		}
	}

	if (!ConfigLayout.empty())
	{
		ImGui::LoadIniSettingsFromDisk(ConfigLayout.c_str());
	}
	else
	{
		ImGui::LoadIniSettingsFromMemory(DefaultEditorImGuiLayout, strlen(DefaultEditorImGuiLayout));
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_MainWindow->GetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init();
}

void
Engine::DestroyImGuiContext()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(m_ImGuiContext);
	m_ImGuiContext = nullptr;
}

Engine *
Engine::GetEngine()
{
	assert(g_Engine != nullptr);
	return g_Engine;
}

void
Engine::LoadProject(const std::string &Path)
{
	if (m_RootConcept != nullptr)
	{
		delete m_RootConcept;
		m_RootConcept = nullptr;
	}

	m_ActiveProject->LoadProject(Path);

	const auto &RootConcepts = m_ActiveProject->GetConfig().root_concept;

	if (!RootConcepts.empty())
	{
		spdlog::info("Loading root concepts: {}", RootConcepts);

		m_RootConcept = new DynamicConcept;
		m_RootConcept->Load(std::format(SHARED_BINARY_PATH_FORMAT, RootConcepts), true);
	}
	else
	{
		spdlog::info("No root concepts specified");
	}
}

Project *
Engine::GetProject() const
{
	return m_ActiveProject;
}
