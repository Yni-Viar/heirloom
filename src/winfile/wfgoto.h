#pragma once

#include <windows.h>

void SetCurrentPathOfWindow(LPWSTR szPath);
INT_PTR CALLBACK GotoDirDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
