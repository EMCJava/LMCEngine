//
// Created by samsa on 7/8/2023.
//

#pragma once

#include <string>

class DynamicLibrary
{
public:
	void
	Load(const std::string &Path);

private:
	std::string m_DLLPath;
};
