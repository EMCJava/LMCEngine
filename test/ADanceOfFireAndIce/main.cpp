#include <Engine/Engine.hpp>

#include <Engine/Core/Concept/Concept.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicLibrary.hpp>

#include <thread>

#include <filesystem>
#include <exception>
#include <iostream>

int
main()
{

	DynamicLibrary Lib;
	assert(Lib.Load(R"(.\build\debug\Source\Foo.CFoo.lib.dll)"));
	auto *mem_alloc = Lib.LoadSymbolAs<void *(*)()>("mem_alloc");
	auto *mem_free = Lib.LoadSymbolAs<void (*)(void *)>("mem_free");

	Lib.ShouldReload();

	auto *c = (Concept *)mem_alloc();
	for (int i = 0; i < 10000000; i++)
	{
		c->Apply();
		std::cout << Lib.ShouldReload() << std::endl;
		if (Lib.ShouldReload())
		{
			mem_free(c);
			Lib.Reload();

			mem_alloc = Lib.LoadSymbolAs<void *(*)()>("mem_alloc");
			mem_free = Lib.LoadSymbolAs<void (*)(void *)>("mem_free");
			c = (Concept *)mem_alloc();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	mem_free(c);

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
