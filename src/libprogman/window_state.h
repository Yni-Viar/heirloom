#pragma once

#include "libprogman/pch.h"

namespace libprogman {

void saveWindowState(HWND hwnd, std::filesystem::path iniFilePath);

bool restoreWindowState(HWND hwnd, std::filesystem::path iniFilePath);

// Save whether a window is minimized to the MinimizedFolderListControl
void saveWindowMinimizedState(std::filesystem::path iniFilePath, bool isMinimized);

// Get whether a window was minimized to the MinimizedFolderListControl
bool getWindowMinimizedState(std::filesystem::path iniFilePath);

}  // namespace libprogman
