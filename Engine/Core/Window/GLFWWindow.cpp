//
// Created by loyus on 7/2/2023.
//

#include "GLFWWindow.hpp"

#include <imgui.h>
// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <Engine/Core/File/OSFile.hpp>

#include <stdexcept>
#include <iostream>

void
InitializeWindowEnvironment()
{
	if (GLFW_FALSE == glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

void
ShutdownWindowEnvironment()
{
	glfwTerminate();
}

Window::Window(int Width, int Height, const char *Title, bool Fullscreen, bool Create)
{
	m_Width = Width;
	m_Height = Height;
	m_Title = Title;
	m_Fullscreen = Fullscreen;

	if (Create)
	{
		CreateWindow();
	}
}

Window::Window(const char *Title, bool Create)
{
	const auto *VideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	m_Width = VideoMode->width;
	m_Height = VideoMode->height;
	m_Title = Title;
	m_Fullscreen = true;

	if (Create)
	{
		CreateWindow();
	}
}

Window::~Window()
{
	if (m_Window != nullptr)
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}
}

void
Window::CreateWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (m_Fullscreen)
	{
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	}

	if (m_Window == nullptr)
	{
		throw std::runtime_error("Failed to create GLFW window");
	}

	MakeContextCurrent();
}

void
Window::Update()
{
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	MakeContextCurrent();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open project"))
			{
				std::cout << "Open project: " << OSFile::PickFile("lmce") << std::endl;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", "Alt+F4"))
			{
				glfwSetWindowShouldClose(m_Window, GL_TRUE);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	{
		ImGui::Begin("Hierarchy");

		ImGui::End();
	}

	{
		ImGui::Begin("Console");
		ImGui::ColorEdit3("clear color", (float *)&clear_color);

		const auto &io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	{
		ImGui::Begin("Details");

		ImGui::End();
	}

	glViewport(0, 0, m_Width, m_Height);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void
Window::SetSwapInterval(int interval)
{
	MakeContextCurrent();
	glfwSwapInterval(interval);
}

void
Window::MakeContextCurrent()
{
	glfwMakeContextCurrent(m_Window);
}

bool
Window::WindowShouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

struct GLFWwindow *
Window::GetWindowHandle() const
{
	return m_Window;
}
