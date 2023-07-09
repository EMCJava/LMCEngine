#include <Engine/Engine.hpp>

#include <Engine/Core/Concept/Concept.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>

#include <thread>

#include <exception>
#include <iostream>
#include <format>

int
main()
{
	Engine engine;
	engine.GetProject()->LoadProject("ADanceOfFireAndIce.lmce");

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
