#include <Engine/Engine.hpp>

#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Runtime/DynamicLibrary/DynamicLibrary.hpp>

#include <exception>
#include <iostream>

int
main()
{

	DynamicLibrary Lib;
	Lib.Load(R"(C:\Users\samsa\CLionProjects\LMCEngine\test\ADanceOfFireAndIce\build\debug\Source\Foo.CFoo.lib.dll)");

	return 0;

	Engine engine;
	engine.GetProject()->LoadProject("ADanceOfFireAndIce.lmce");

	try {
		while (!engine.ShouldShutdown()) {
			engine.Update();
		}
	}
	catch (std::exception &e) {
		std::cerr << "Uncaught exception: " << e.what() << std::endl;
	}

	return 0;
}
