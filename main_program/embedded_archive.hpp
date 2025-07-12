#pragma once

#include <string>
#include <string_view>

std::string ArchiveGetFile(std::string_view arg_filename, std::string &extension, bool prefix_only = false) noexcept;
