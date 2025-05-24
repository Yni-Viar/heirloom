#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutFolder.h"
#include "libprogman/ShortcutManager.h"

namespace progman {

class ProgramManagerWindow;  // Forward declaration

class FolderWindow {
   public:
    FolderWindow(HINSTANCE instance, HWND mdiClient, std::shared_ptr<libprogman::ShortcutFolder> folder);
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

    LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

   private:
    HWND window_ = nullptr;
    HWND listView_ = nullptr;
    std::shared_ptr<libprogman::ShortcutFolder> folder_;
    std::function<void(const std::wstring&)> onMinimizeCallback_;

    void createListView();
    void refreshListView();
    friend LRESULT CALLBACK FolderWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    friend class ProgramManagerWindow;  // Allow ProgramManagerWindow to access window_
};

}  // namespace progman
