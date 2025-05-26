#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutFactory.h"
#include "libprogman/Shortcut.h"

namespace progman {

class NewShortcutDialog {
   public:
    NewShortcutDialog(
        HWND parentWindow,
        HINSTANCE hInstance,
        std::filesystem::path folder,
        immer::vector<std::shared_ptr<libprogman::Shortcut>> installedApps,
        libprogman::ShortcutFactory* shortcutFactory);

    void showDialog();

   private:
    static INT_PTR CALLBACK dialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void initializeDialog(HWND hwnd);
    void populateApplicationsList(HWND hwnd);
    void handleOkButton(HWND hwnd);
    void handleBrowseButton(HWND hwnd);
    void selectApplicationRadio(HWND hwnd);
    void selectPathRadio(HWND hwnd);
    bool validateInputs(HWND hwnd);
    void createShortcut(HWND hwnd);

    HWND parentWindow_;
    HINSTANCE hInstance_;
    std::filesystem::path folder_;
    immer::vector<std::shared_ptr<libprogman::Shortcut>> installedApps_;
    libprogman::ShortcutFactory* shortcutFactory_;
    HWND dialogHandle_ = nullptr;
};

}  // namespace progman
