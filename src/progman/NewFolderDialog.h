#pragma once

#include "progman/pch.h"
#include "libprogman/ShortcutManager.h"

namespace progman {

class NewFolderDialog {
   public:
    NewFolderDialog(HWND parentWindow, HINSTANCE hInstance, libprogman::ShortcutManager* shortcutManager);

    // Shows the dialog modally and returns true if a folder was created
    bool show();

   private:
    static INT_PTR CALLBACK dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    INT_PTR handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool validateAndCreateFolder(HWND hwnd);

    HWND parentWindow_;
    HINSTANCE hInstance_;
    libprogman::ShortcutManager* shortcutManager_;
};

}  // namespace progman
