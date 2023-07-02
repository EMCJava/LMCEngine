//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <spdlog/spdlog.h>

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

	spdlog::info("Engine initializing");
	InitializeEnvironment();

	m_MainWindowPool = new WindowPool;

	m_MainWindowPool->CreateWindowFullscreen("LMCEngine");

	g_Engine = this;
}

Engine::~Engine()
{
	spdlog::info("Engine destroying");

	delete m_MainWindowPool;
	m_MainWindowPool = nullptr;

	ShutdownEnvironment();
}

void
Engine::Update()
{
	m_MainWindowPool->Update();

	m_ShouldShutdown |= m_MainWindowPool->PrimaryShouldClose();
}

bool
Engine::ShouldShutdown()
{
	return m_ShouldShutdown;
}
