//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <Engine/Core/Environment/Environment.hpp>

#include <iostream>

Engine::Engine()
{
	std::cout << "Engine initializing" << std::endl;
	InitializeEnvironment();

	m_MainWindow = new Window(640, 480, "LMCEngine");
	if (m_MainWindow == nullptr)
	{
		throw std::runtime_error("Failed to allocate window");
	}

	m_MainWindow->SetSwapInterval(1);
}

Engine::~Engine()
{
	std::cout << "Engine destroying" << std::endl;

	delete m_MainWindow;

	ShutdownEnvironment();
}

void
Engine::Update()
{
	m_MainWindow->Update();
	m_ShouldShutdown |= m_MainWindow->WindowShouldClose();
}

bool
Engine::ShouldShutdown()
{
	return m_ShouldShutdown;
}
