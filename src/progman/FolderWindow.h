#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutFolder.h"
#include "libprogman/ShortcutManager.h"

namespace progman {

class ProgramManagerWindow;  // Forward declaration

// Define a custom message for delete command
const UINT WM_FOLDERWINDOW_DELETE = WM_USER + 100;

class FolderWindow {
   public:
    FolderWindow(
        HINSTANCE instance,
        HWND mdiClient,
        std::shared_ptr<libprogman::ShortcutFolder> folder,
        libprogman::ShortcutManager* shortcutManager);
    FolderWindow(const FolderWindow&) = delete;
    FolderWindow& operator=(const FolderWindow&) = delete;
    FolderWindow(FolderWindow&&) = delete;
    FolderWindow& operator=(FolderWindow&&) = delete;

    void setFolder(std::shared_ptr<libprogman::ShortcutFolder> folder);
    void show();
    void close();
    void setOnMinimizeCallback(std::function<void(const std::wstring&)> callback);
    std::wstring getName() const;

    // New methods for handling item selection
    bool hasSelectedItem() const;
    libprogman::Shortcut* getSelectedShortcut() const;
    void renameSelectedItem();

    // Public method to save window state
    void saveState();

    // Methods to handle minimized state
    void setMinimized(bool minimized);
    bool isMinimized() const;
    bool wasMinimizedOnSave() const;

    // Handles delete command for the selected shortcut or folder
    void handleDeleteCommand();

    LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

   private:
    HINSTANCE instance_ = nullptr;
    HWND window_ = nullptr;
    HWND listView_ = nullptr;
    std::shared_ptr<libprogman::ShortcutFolder> folder_;
    libprogman::ShortcutManager* shortcutManager_ = nullptr;
    std::function<void(const std::wstring&)> onMinimizeCallback_;
    bool isMinimized_ = false;

    void createListView();
    void refreshListView();

    // Helper functions for window state
    void saveWindowState(HWND hwnd);
    void restoreWindowState(HWND hwnd);
    std::optional<std::filesystem::path> getIniFilePath() const;

    friend LRESULT CALLBACK FolderWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    friend class ProgramManagerWindow;  // Allow ProgramManagerWindow to access window_
};

}  // namespace progman
