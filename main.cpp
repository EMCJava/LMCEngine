#include <Engine/Engine.hpp>

#include <exception>
#include <iostream>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

int
main()
{

	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow *window = glfwCreateWindow(640, 480, "LMCEngine", nullptr, nullptr);
	if (!window)
	{
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

	/////////////////////////

	Engine engine;

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
