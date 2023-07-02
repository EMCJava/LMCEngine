//
// Created by loyus on 7/2/2023.
//

#include "WindowPool.hpp"

#include <Engine/Core/Window/Window.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>

WindowPool::~WindowPool()
{
	for (auto &window: m_Windows)
	{
		delete window;
	}
}

void
WindowPool::Update()
{
	if (m_Windows.empty()) [[unlikely]]
	{
		return;
	}

	for (size_t i = 0; i < m_Windows.size(); i++)
	{
		m_Windows[i]->Update();
		if (m_Windows[i]->WindowShouldClose()) [[unlikely]]
		{
			delete m_Windows[i];
			m_Windows.erase(m_Windows.begin() + i--);
		}
	}

	for (auto Window: m_Windows)
	{
		glfwSwapBuffers(Window->GetWindowHandle());
	}
}

bool
WindowPool::ShouldClose() const
{
	return m_Windows.empty();
}

std::pair<int, int>
WindowPool::GetPrimaryMonitorMode() const
{
	const auto *VideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return {VideoMode->width, VideoMode->height};
}