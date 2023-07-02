//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <string>

struct ProjectConfig {
	std::string project_name;
};

class Project
{
public:

	Project() = default;
	~Project();

	void
	LoadProject(const std::string &ProjectFilePath);

	ProjectConfig &
	GetConfig();

private:
	ProjectConfig m_Config;
};
