//
// Created by samsa on 7/8/2023.
//

#include "DynamicLibrary.hpp"

#include <Engine/Core/Core.hpp>

#include <spdlog/spdlog.h>

#ifdef LMC_WIN
#	include <windows.h>
#else
#	error not implemented yet
#endif

void
DynamicLibrary::Load(const std::string &Path)
{
	m_DLLPath = Path;

	void *_handle;
	_handle = LoadLibrary(m_DLLPath.c_str());

	if (!_handle) {
		spdlog::error("Can't open and load [{}]", m_DLLPath);
	}
}
