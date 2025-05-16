#pragma once

#include <windows.h>

BOOL IsRecycleBinEmpty();
BOOL GetRecycleBinSize(PLARGE_INTEGER pliSize);
BOOL EmptyRecycleBin(HWND hwnd);
DWORD MoveFileToRecycleBin(LPWSTR pszFile);
void FormatRecycleBinSize(PLARGE_INTEGER pliSize, LPWSTR szBuffer);

extern BOOL bRecycleBinEmpty;
extern LARGE_INTEGER qRecycleBinSize;
