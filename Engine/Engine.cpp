//
// Created by loyus on 7/1/2023.
//

#include "Engine.hpp"

#include <iostream>

Engine::Engine()
{
	std::cout << "Engine initializing" << std::endl;
}

Engine::~Engine()
{
	std::cout << "Engine destroying" << std::endl;
}

void
Engine::Update()
{
	std::cout << "Engine update" << std::endl;
}

bool
Engine::ShouldShutdown()
{
	return m_shouldShutdown;
}
