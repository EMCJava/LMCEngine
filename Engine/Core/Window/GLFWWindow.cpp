//
// Created by loyus on 7/2/2023.
//

#include "GLFWWindow.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

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
