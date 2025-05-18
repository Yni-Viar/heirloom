#pragma once

#include "libprogman/pch.h"

namespace libprogman {

std::wstring utf8ToWide(const std::string& str);
std::string wideToUtf8(const std::wstring& wstr);

}  // namespace libprogman
