#pragma once

#include <windows.h>

void GetSearchPath(HWND hwnd, LPWSTR szTemp);
INT_PTR CALLBACK SearchProgDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateSearchStatus(HWND hwndLB, int nCount);
void SearchEnd();
