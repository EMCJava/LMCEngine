//
// Created by loyus on 7/2/2023.
//

#include "Project.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProjectConfig, project_name, root_concept)

ProjectConfig &
Project::GetConfig()
{
	return m_Config;
}

void
Project::LoadProject(const std::string &ProjectFilePath)
{
	m_ProjectFilePath = "";
	spdlog::info("Reset project, Open project: [{}]", ProjectFilePath);

	if (!ProjectFilePath.ends_with(".lmce"))
	{
		spdlog::warn("Unknown file extension: {}", ProjectFilePath);
		return;
	}

	std::ifstream ifs(ProjectFilePath);
	m_Config = nlohmann::json::parse(ifs);

	spdlog::info("Project name: {}", m_Config.project_name);

	if (m_Config.editor_layout_path.empty())
	{
		std::filesystem::path Path(ProjectFilePath);

		const auto EditorLayoutPath = Path / "/Editor/layout.ini";
		if (std::filesystem::exists(EditorLayoutPath))
		{
			m_Config.editor_layout_path = EditorLayoutPath.string();
		}
	}

	m_ProjectFilePath = ProjectFilePath;
}

void
Project::SaveProject()
{
	if (m_ProjectFilePath.empty())
	{
		spdlog::warn("Project path empty, unable to save project");
		return;
	}

	std::ofstream ofs(m_ProjectFilePath);

	if (!ofs)
	{
		spdlog::error("Unable to open project path: [{}]", m_ProjectFilePath);
		return;
	}

	ofs << nlohmann::json{m_Config}[0].dump(4);
}

Project::~Project()
{
	spdlog::info("Close project: {}", m_Config.project_name);
}