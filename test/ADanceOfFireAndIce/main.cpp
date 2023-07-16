#include <Engine/Engine.hpp>

#include <Engine/Core/Concept/Concept.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>

#include <thread>

#include <exception>
#include <iostream>

int
main()
{
	Engine engine;

	try
	{
		while (!engine.ShouldShutdown())
		{
			engine.Update();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Uncaught exception: " << e.what() << std::endl;
	}

	return 0;
}
