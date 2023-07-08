//
// Created by loyus on 7/2/2023.
//

#include "OSFile.hpp"

#include <nfd.h>
#include <stdexcept>

std::string
OSFile::PickFile(const char *filter)
{
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_OpenDialog(filter, nullptr, &outPath);

	if (result == NFD_OKAY)
	{
		std::string path(outPath);
		free(outPath);

		return path;
	}
	else if (result == NFD_CANCEL)
	{
		return "";
	}
	else
	{
		throw std::runtime_error(std::string("OSFile::PickFile error: ") + NFD_GetError());
	}
}

std::string
OSFile::SaveFile(const char *filter)
{
	nfdchar_t *outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog(filter, nullptr, &outPath);

	if (result == NFD_OKAY)
	{
		std::string path(outPath);
		free(outPath);

		return path;
	}
	else if (result == NFD_CANCEL)
	{
		return "";
	}
	else
	{
		throw std::runtime_error(std::string("OSFile::PickFile error: ") + NFD_GetError());
	}
}
