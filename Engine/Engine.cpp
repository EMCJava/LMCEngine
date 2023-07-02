//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <Engine/Core/Environment/Environment.hpp>
#include <Engine/Core/Window/WindowPool.hpp>

#include <iostream>

Engine *Engine::g_Engine = nullptr;

Engine::Engine()
{
	if (g_Engine != nullptr)
	{
		throw std::runtime_error("There is already a Engine instance!");
	}

	std::cout << "Engine initializing" << std::endl;
	InitializeEnvironment();

	m_MainWindowPool = new WindowPool;

	m_MainWindowPool->CreateWindow(640, 480, "LMCEngine1");
	m_MainWindowPool->CreateWindow(640, 480, "LMCEngine2");

	g_Engine = this;
}

Engine::~Engine()
{
	std::cout << "Engine destroying" << std::endl;

	delete m_MainWindowPool;
	m_MainWindowPool = nullptr;

	ShutdownEnvironment();
}

void
Engine::Update()
{
	m_MainWindowPool->Update();

	m_ShouldShutdown |= m_MainWindowPool->AllShouldClose();
}

bool
Engine::ShouldShutdown()
{
	return m_ShouldShutdown;
}
