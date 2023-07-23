//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <nfd.hpp>

#include <string>

namespace OSFile
{

std::string
PickFile(const std::vector<nfdfilteritem_t>& filter, const char *default_path = nullptr);

std::string
SaveFile(const std::vector<nfdfilteritem_t>& filter, const char *default_path = nullptr);

};// namespace OSFile
