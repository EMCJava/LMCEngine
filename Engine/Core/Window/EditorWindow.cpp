//
// Created by loyus on 7/2/2023.
//

#include "EditorWindow.hpp"

#include <imgui.h>

// #define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include <Engine/Engine.hpp>
#include <Engine/Core/File/OSFile.hpp>
#include <Engine/Core/Project/Project.hpp>
#include <Engine/Core/Exception/Runtime/ImGuiContextInvalid.hpp>

void
EditorWindow::Update()
{
	if (RootConcept != nullptr)
	{
		RootConcept->Apply();
	}

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
				const auto ProjectPath = OSFile::PickFile("lmce");
				if (ProjectPath.empty())
				{
					spdlog::info("Operation cancelled");
				}
				else
				{
					Engine::GetEngine()->GetProject()->LoadProject(ProjectPath);
				}
			}

			if (ImGui::MenuItem("Save project"))
			{
				spdlog::info("Save project");
				Engine::GetEngine()->GetProject()->SaveProject();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", "Alt+F4"))
			{
				glfwSetWindowShouldClose(m_Window, GL_TRUE);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::BeginMenu("Layout"))
			{
				auto &ImGuiIO = ImGui::GetIO();
				if (ImGui::MenuItem("Save Layout", nullptr, false, ImGuiIO.WantSaveIniSettings))
				{
					const auto SaveLocation = OSFile::SaveFile("ini");
					if (!SaveLocation.empty())
					{
						ImGui::SaveIniSettingsToDisk(SaveLocation.c_str());
						ImGuiIO.WantSaveIniSettings = false;
					}
				}

				if (ImGui::MenuItem("Load Layout"))
				{
					const auto LoadLocation = OSFile::PickFile("ini");
					if (!LoadLocation.empty())
					{
						Engine::GetEngine()->GetProject()->GetConfig().editor_layout_path = LoadLocation;
						throw ImGuiContextInvalid{};
					}
				}

				ImGui::EndMenu();
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
