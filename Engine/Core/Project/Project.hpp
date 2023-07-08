//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <string>

struct ProjectConfig {
	std::string project_name = "Placeholder Project";
	std::string root_concept;
	std::string editor_layout_path;
};

class Project
{
public:
	Project() = default;
	~Project();

	void
	LoadProject(const std::string &ProjectFilePath);

	void
	SaveProject();

	ProjectConfig &
	GetConfig();

private:
	ProjectConfig m_Config;
	std::string m_ProjectFilePath;
};
