#pragma once

#include "progman/pch.h"

namespace progman {

class ProgramManagerWindow {
   public:
    void create();
    void show(int cmdShow);

    HWND getHwnd() const { return hwnd_; }

   private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void registerWindowClass();
    void createMdiClient();

    HWND hwnd_ = nullptr;
    HWND mdiClient_ = nullptr;
    WNDCLASSEXW windowClass_{};
    HINSTANCE hInstance_ = nullptr;
};

}  // namespace progman
