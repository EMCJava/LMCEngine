//
// Created by loyus on 7/2/2023.
//

#include "Project.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, project_name)

ProjectConfig &
Project::GetConfig()
{
	return m_Config;
}

void
Project::LoadProject(const std::string &ProjectFilePath)
{
	spdlog::info("Open project: {}", ProjectFilePath);

	if (!ProjectFilePath.ends_with(".lmce"))
	{
		spdlog::warn("Unknown file extension: {}", ProjectFilePath);
		return;
	}

	std::ifstream ifs(ProjectFilePath);
	m_Config = nlohmann::json::parse(ifs);

	spdlog::info("Project name: {}", m_Config.project_name);

	std::filesystem::path Path(ProjectFilePath);

	const auto EditorLayoutPath = Path / "/Editor/layout.ini";
	if (std::filesystem::exists(EditorLayoutPath))
	{
		m_Config.editor_layout_path = EditorLayoutPath.string();
	}
}

Project::~Project()
{
	spdlog::info("Close project: {}", m_Config.project_name);
}
