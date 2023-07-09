#include <Engine/Engine.hpp>

#include <Engine/Core/Concept/Concept.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicConcept.hpp>

#include <thread>

#include <filesystem>
#include <exception>
#include <iostream>

int
main()
{

	DynamicConcept DConcept;
	DConcept.Load(R"(.\build\debug\Source\Foo.CFoo.lib.dll)", true);

	for (int i = 0; i < 10; i++)
	{
		DConcept->Apply();
		if (DConcept.ShouldReload())
		{
			DConcept.Reload();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return 0;

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
