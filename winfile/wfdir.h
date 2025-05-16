#pragma once

#include <windows.h>
#include "wfmem.h"

void UpdateStatus(HWND hWnd);
LPWSTR DirGetSelection(HWND hwndDir, HWND hwndView, HWND hwndLB, int iSelType, BOOL* pfDir, PINT piLastSel);
void FillDirList(HWND hwndDir, LPXDTALINK lpStart);
void CreateLBLine(DWORD dwLineFormat, LPXDTA lpxdta, LPWSTR szBuffer);
int GetMaxExtent(HWND hwndLB, LPXDTALINK lpXDTA, BOOL bNTFS);
void UpdateSelection(HWND hwndLB);

int PutDate(LPFILETIME lpftDate, LPWSTR szStr);
int PutTime(LPFILETIME lpftTime, LPWSTR szStr);
int PutSize(PLARGE_INTEGER pqSize, LPWSTR szOutStr);
int PutAttributes(DWORD dwAttribute, LPWSTR szStr);
HWND GetMDIChildFromDescendant(HWND hwnd);
void SetLBFont(HWND hwnd, HWND hwndLB, HANDLE hNewFont, DWORD dwViewFlags, LPXDTALINK lpStart);
LRESULT CALLBACK DirWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DirListBoxWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
