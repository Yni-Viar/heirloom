#include "libprogman/pch.h"

namespace libprogman {

void saveWindowState(HWND hwnd, std::filesystem::path iniFilePath);

void restoreWindowState(HWND hwnd, std::filesystem::path iniFilePath);

}  // namespace libprogman
