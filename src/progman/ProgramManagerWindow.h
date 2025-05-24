#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutManager.h"
#include "progman/FolderWindow.h"

namespace progman {

class ProgramManagerWindow {
   public:
    ProgramManagerWindow(HINSTANCE hInstance, libprogman::ShortcutManager* shortcutManager);
    ProgramManagerWindow(const ProgramManagerWindow&) = delete;
    ProgramManagerWindow& operator=(const ProgramManagerWindow&) = delete;
    ProgramManagerWindow(ProgramManagerWindow&&) = delete;
    ProgramManagerWindow& operator=(ProgramManagerWindow&&) = delete;

    void show(int cmdShow);
    void refresh();
    HWND hwnd() const { return hwnd_; }

   private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void registerWindowClass();
    void createMdiClient();
    void syncFolderWindows();

    HWND hwnd_ = nullptr;
    HWND mdiClient_ = nullptr;
    WNDCLASSEXW windowClass_{};
    HINSTANCE hInstance_ = nullptr;
    libprogman::ShortcutManager* shortcutManager_ = nullptr;
    std::unordered_map<std::wstring, std::unique_ptr<FolderWindow>> folderWindows_;
};

}  // namespace progman
