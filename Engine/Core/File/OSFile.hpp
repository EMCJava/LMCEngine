//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <string>

namespace OSFile
{

std::string
PickFile(const char *filter, const char *default_path = nullptr);

std::string
SaveFile(const char *filter, const char *default_path = nullptr);

};// namespace OSFile
