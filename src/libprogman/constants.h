#pragma once

#include "libprogman/pch.h"

namespace libprogman {

// FolderWindow: Delete and Rename menu commands.
constexpr UINT WM_FOLDERWINDOW_DELETE = WM_APP + 1;
constexpr UINT WM_FOLDERWINDOW_RENAME = WM_APP + 2;

// FindingAppsDialog: Custom message for displaying errors from the worker thread.
constexpr UINT WM_DISPLAY_ERROR = WM_APP + 3;

// ProgramManagerWindow: Sent from worker thread to UI thread to trigger sync of folder windows.
constexpr UINT WM_SYNC_FOLDER_WINDOWS = WM_APP + 4;

}  // namespace libprogman
