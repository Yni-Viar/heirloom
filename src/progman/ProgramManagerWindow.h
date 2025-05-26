#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutManager.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/InstalledAppList.h"
#include "progman/FolderWindow.h"
#include "progman/MinimizedFolderListControl.h"

namespace progman {

class ProgramManagerWindow {
   public:
    ProgramManagerWindow(
        HINSTANCE hInstance,
        libprogman::ShortcutManager* shortcutManager,
        libprogman::ShortcutFactory* shortcutFactory,
        libprogman::InstalledAppList* installedAppList);
    ProgramManagerWindow(const ProgramManagerWindow&) = delete;
    ProgramManagerWindow& operator=(const ProgramManagerWindow&) = delete;
    ProgramManagerWindow(ProgramManagerWindow&&) = delete;
    ProgramManagerWindow& operator=(ProgramManagerWindow&&) = delete;

    void show(int cmdShow);
    void refresh();
    HWND hwnd() const { return hwnd_; }

    FolderWindow* getActiveFolderWindow() const;

   private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void registerWindowClass();
    void createMdiClient();
    void syncFolderWindows();
    void restoreMinimizedFolder(const std::wstring& folderName, bool maximize = false);
    void handleDeleteCommand();

    // Methods to save/load splitter position
    void saveSplitterPosition() const;
    void loadSplitterPosition();

    // Method to sort window menu items alphabetically
    void sortWindowMenu(HMENU windowMenu);

    HWND hwnd_ = nullptr;
    HWND mdiClient_ = nullptr;
    WNDCLASSEXW windowClass_{};
    HINSTANCE hInstance_ = nullptr;
    libprogman::ShortcutManager* shortcutManager_ = nullptr;
    libprogman::ShortcutFactory* shortcutFactory_ = nullptr;
    libprogman::InstalledAppList* installedAppList_ = nullptr;
    std::unordered_map<std::wstring, std::unique_ptr<FolderWindow>> folderWindows_;
    std::unique_ptr<MinimizedFolderListControl> minimizedFolderList_;
};

}  // namespace progman
