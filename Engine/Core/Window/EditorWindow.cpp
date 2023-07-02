//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <imgui.h>
// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Engine/Core/File/OSFile.hpp>

void
EditorWindow::Update()
{
	MakeContextCurrent();

	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	glViewport(0, 0, m_Width, m_Height);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open project"))
			{
				spdlog::info("Open project");
				const auto Path = OSFile::PickFile("lmce");
				if (Path.empty()) {
					spdlog::info("Operation cancelled");
				}
				else {
					spdlog::info("Open project: {}", Path);
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", "Alt+F4"))
			{
				glfwSetWindowShouldClose(m_Window, GL_TRUE);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	{
		ImGui::Begin("Hierarchy");

		ImGui::End();
	}

	{
		ImGui::Begin("Console");
		ImGui::ColorEdit3("clear color", (float *)&clear_color);

		const auto &io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	{
		ImGui::Begin("Details");

		ImGui::End();
	}
}
