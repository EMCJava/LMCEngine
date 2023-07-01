//
// Created by loyus on 7/2/2023.
//

#include "GLFWWindow.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

void
InitializeWindowEnvironment()
{
	if (GLFW_FALSE == glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

void
ShutdownWindowEnvironment()
{
	glfwTerminate();
}

Window::Window(int Width, int Height, const char *Title, bool Fullscreen, bool Create)
{
	m_Width = Width;
	m_Height = Height;
	m_Title = Title;
	m_Fullscreen = Fullscreen;

	if (Create)
	{
		CreateWindow();
	}
}

Window::~Window()
{
	if (m_ImGuiContext != nullptr) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_ImGuiContext);
		m_ImGuiContext = nullptr;
	}

	delete m_GLContext;

	if (m_Window != nullptr)
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}
}

void
Window::CreateWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (m_Fullscreen)
	{
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	}

	if (m_Window == nullptr)
	{
		throw std::runtime_error("Failed to create GLFW window");
	}

	MakeContextCurrent();

	m_GLContext = new GladGLContext;
	[[maybe_unused]] int version = gladLoadGLContext(m_GLContext, glfwGetProcAddress);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	m_ImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_ImGuiContext);
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();
}

void
Window::Update()
{
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	MakeContextCurrent();
	glfwPollEvents();

	// Start the Dear ImGui frame
	ImGui::SetCurrentContext(m_ImGuiContext);
	// ImGui_ImplOpenGL3_Init(); // recreate GL objects on parent viewer

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");// Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");// Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *)&clear_color);// Edit 3 floats representing a color

		if (ImGui::Button("Button"))// Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		const auto &io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	ImGui::Render();
	m_GLContext->Viewport(0, 0, m_Width, m_Height);
	m_GLContext->ClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	m_GLContext->Clear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Window);
}

void
Window::SetSwapInterval(int interval)
{
	MakeContextCurrent();
	glfwSwapInterval(interval);
}

void
Window::MakeContextCurrent()
{
	glfwMakeContextCurrent(m_Window);
}

bool
Window::WindowShouldClose()
{
	return glfwWindowShouldClose(m_Window);
}
