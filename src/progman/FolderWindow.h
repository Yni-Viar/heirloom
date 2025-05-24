#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutFolder.h"

namespace progman {

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

   private:
    HWND window_ = nullptr;
    HWND listView_ = nullptr;
    std::shared_ptr<libprogman::ShortcutFolder> folder_;

    void createListView();
    void refreshListView();
    LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    friend LRESULT CALLBACK FolderWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

}  // namespace progman
