//
// Created by loyus on 7/2/2023.
//

#include "WindowPool.hpp"

#include <Engine/Core/Window/Window.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <stdexcept>

WindowPool::~WindowPool()
{
	for (auto &window: m_Windows)
	{
		delete window;
	}

	if (m_ImGuiContext != nullptr)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_ImGuiContext);
		m_ImGuiContext = nullptr;
	}
}

Window *
WindowPool::CreateWindow(int Width, int Height, const char *Title, bool Fullscreen, bool Create)
{
	const bool FirstWindow = m_Windows.empty();
	if (!Create && FirstWindow)
	{
		throw std::runtime_error("WindowPool::CreateWindow: First window should always be created for glad and ImGui to initialize.");
	}

	auto *NewWindow = new Window(Width, Height, Title, Fullscreen, Create);
	if (NewWindow == nullptr)
	{
		throw std::runtime_error("Failed to allocate window");
	}

	NewWindow->SetSwapInterval(1);
	m_Windows.push_back(NewWindow);

	if (!FirstWindow)
	{
		return NewWindow;
	}

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

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(NewWindow->GetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init();

	return NewWindow;
}

Window *
WindowPool::CreateWindowFullscreen(const char *Title, bool Create)
{
	const auto *VideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return CreateWindow(VideoMode->width, VideoMode->height, Title, true, Create);
}

void
WindowPool::Update()
{
	if (m_Windows.empty()) [[unlikely]]
	{
		return;
	}

	glfwPollEvents();

	m_Windows.front()->MakeContextCurrent();
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	for (size_t i = 0; i < m_Windows.size(); i++)
	{
		m_Windows[i]->Update();
		if (i != 0 && m_Windows[i]->WindowShouldClose()) [[unlikely]]
		{
			delete m_Windows[i];
			m_Windows.erase(m_Windows.begin() + i--);
		}
	}

	m_Windows.front()->MakeContextCurrent();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	for (auto Window: m_Windows)
	{
		glfwSwapBuffers(Window->GetWindowHandle());
	}
}

bool
WindowPool::PrimaryShouldClose() const
{
	return m_Windows.empty() || m_Windows.front()->WindowShouldClose();
}
