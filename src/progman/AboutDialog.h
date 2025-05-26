#pragma once

#include "progman/pch.h"

namespace progman {

class AboutDialog {
   public:
    AboutDialog(HWND parentWindow, HINSTANCE hInstance);
    void showDialog();

   private:
    static INT_PTR CALLBACK dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    friend INT_PTR CALLBACK dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void initDialog(HWND hwnd);
    void loadVersionInfo(HWND hwnd);

    HWND parentWindow_;
    HINSTANCE hInstance_;
};

}  // namespace progman