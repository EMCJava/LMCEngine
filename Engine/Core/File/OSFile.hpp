//
// Created by loyus on 7/2/2023.
//

#pragma once

#include <nfd.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace OSFile
{

std::string
PickFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path = nullptr );

std::string
SaveFile( const std::vector<nfdfilteritem_t>& filter, const char* default_path = nullptr );

std::string
PickFolder( const char* default_path = nullptr );

bool
CopyFolder( const std::filesystem::path& Path, std::filesystem::path Destination );

};   // namespace OSFile
